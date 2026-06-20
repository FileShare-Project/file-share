/*
** Project FileShare, 2025
**
** Author Francois Michaut
**
** Started on  Wed Jul 23 13:49:52 2025 Francois Michaut
** Last update Wed May 27 12:06:41 2026 Francois Michaut
**
** interactive.cpp : Interractive Mode logic
*/

#include "FileShare/Peer/Peer.hpp"
#include "FileShare/Peer/PeerBase.hpp"
#include "FileShare/Peer/PreAuthPeer.hpp"
#include "FileShareVersion.hpp"

#include <FileShare/Config/ServerConfig.hpp>
#include <FileShare/Protocol/Definitions.hpp>
#include <FileShare/Server.hpp>
#include <FileShare/Utils/Path.hpp>
#include <FileShare/Utils/Poll.hpp>
#include <FileShare/Utils/Strings.hpp>

#include <CppSockets/IPv4.hpp>

#include <csignal>
#include <cstddef>
#include <iostream>
#include <memory>
#include <sstream>
#include <unistd.h>

/* NOLINTBEGIN */
extern bool server_run;
void signal_handler(int sig);

void string_trim(std::string &str);
void string_lowercase(std::string &str);

void interactive_mode(FileShare::Server &server);
auto interactive_user_question(const std::string_view &question) -> bool;
auto interactive_user_input(const std::string_view &question) -> std::string;
/* NOLINTEND */

enum class InteractiveStateMachine : std::uint8_t {
    MAIN_MENU       = 0x00,
    EXIT_MENU       = 0x01,
    CONFIG_MENU     = 0x05,

    DISCONNECT_MENU = 0x12,
    ACCEPT_MENU     = 0x13,
    SELECT_MENU     = 0x16,

    EXECUTE_MENU    = 0x20,

    TRANSFERS_MENU  = 0x30,
};

static auto operator<<(std::ostream &out, InteractiveStateMachine state) -> std::ostream & {
    switch (state) {
        case InteractiveStateMachine::MAIN_MENU:
            out << "MAIN";
            break;
        case InteractiveStateMachine::EXIT_MENU:
            out << "EXIT";
            break;
        case InteractiveStateMachine::CONFIG_MENU:
            out << "CONFIG";
            break;
        case InteractiveStateMachine::DISCONNECT_MENU:
            out << "DISCONNECT";
            break;
        case InteractiveStateMachine::ACCEPT_MENU:
            out << "ACCEPT";
            break;
        case InteractiveStateMachine::SELECT_MENU:
            out << "SELECT";
            break;
        case InteractiveStateMachine::EXECUTE_MENU:
            out << "EXECUTE";
            break;
        case InteractiveStateMachine::TRANSFERS_MENU:
            out << "TRANSFERS";
            break;
        default:
            out << "Unkown State: " << static_cast<int>(state);
    }

    return out;
}

struct SubMenuCommandState {
    std::string subcommand;
    std::stringstream args;
};

struct InteractiveState {
    InteractiveState(FileShare::Server &server) : server(server) {}

    bool should_prompt = true;

    FileShare::Server &server;
    std::vector<FileShare::Server::Event> pending_events;
    std::weak_ptr<FileShare::Peer> selected_peer;

    // TODO: That's a HACK. Find a better solution
    // Idea: Give a unique auto-increment int ID to each peer instance ?
    std::vector<FileShare::Peer_ptr> peers;
    std::vector<FileShare::PreAuthPeer_ptr> pending_peers;

    InteractiveStateMachine state = InteractiveStateMachine::MAIN_MENU;
    SubMenuCommandState sub_menu_state;
};

using CommandMap = std::unordered_map<std::string, std::function<void(InteractiveState &, std::stringstream &)>>;
using ExecuteCommandMap = std::unordered_map<std::string, std::function<void(FileShare::Peer_ptr &, InteractiveState &, std::stringstream &)>>;
using ConfigCommandMap = std::unordered_map<std::string, std::function<void(InteractiveState &, std::string_view, std::stringstream &)>>;

static void display_interactive_help() {
    std::cout << "Available Commands are :\n";
    std::cout << "\t" << "- HELP: Display this help message.\n";
    std::cout << "\t" << "- EXIT|QUIT: Stop the program.\n";
    std::cout << "\t" << "- SERVER (ON|OFF): Toggle the Server ON or OFF.\n\t\t If Server is OFF, "
        "no external peers can connect, but you can still initiate connections (existing "
        "connections will not be terminated).\n";

    std::cout << "\t" << "- CONFIG (SERVER|DEFAULT|<PEER_ID>) [SUBCOMMAND] [<ARG>]...: "
        "(Interractive) Modify configuration on the fly. Available subcommands :\n";
    std::cout << "\t\t" << "- SHOW: (default) Display the selected configuration.\n";
    std::cout << "\t\t" << "- SET <NAME> <VALUE>: Set a simple configuration value.\n";
    std::cout << "\t\t" << "- SAVE [<PATH>]: Save the config to a file at PATH. "
        "Overwrites file if exists. If PATH is empty, will save into the original file.\n";
    std::cout << "\t\t" << "- LOAD [<PATH>]: Replace the current config with the one in the "
        "config file at PATH. If PATH is empty, will load from the default location.\n";
    // TODO: FileMap config support

    std::cout << "\t" << "- PEER <SUBCOMMAND> [<ARG>]...: Manage peers. Available subcommands :\n";
    std::cout << "\t\t" << "- LIST: List currently connected peers.\n";
    std::cout << "\t\t" << "- CONNECT <IP> [<PORT>]: Connect to a new peer.\n";
    std::cout << "\t\t" << "- DISCONNECT [<ID>]: (Interractive) Disconnect from a connected peer "
        "or reject a connection request.\n";
    std::cout << "\t\t" << "- ACCEPT [<ID>]: (Interractive) Accept a connection request from an "
        "external peer.\n";
    std::cout << "\t\t" << "- SELECT [<ID>]: (Interractive) Promote a peer as the 'selected' one."
        "\n\t\t\t If connected to multiple peers, pre-selecting a peer allows to send multiple "
        "commands without having to select it each time.\n";

    // TODO: Where do I select Peer if multiple and None selected ?
    std::cout << "\t" << "- EXECUTE <COMMAND> [<ARG>]...: (Interractive) Execute a command on a "
        "selected peer. Will prompt to select a peer if none selected. Available commands :\n";
    std::cout << "\t\t" << "- LIST_FILES [<PATH>]: List availables files on the peer. Can be "
        "Downloaded using RECEIVE_FILE.\n";
    std::cout << "\t\t" << "- RECEIVE_FILE <REMOTE_PATH>: Request a file download\n";
    std::cout << "\t\t" << "- SEND_FILE <LOCAL_PATH>: Send a file to the peer.\n";

    std::cout << "\t" << "- TRANSFERS (LIST|ACCEPT): (Interractive) Display or manage transfers:\n";
    std::cout << "\t\t" << "- LIST: Display ongoing Downloads and Uploads transfers and their "
        "progress, as well as pending transfer requests.\n";
    std::cout << "\t\t" << "- ACCEPT <ID>: Accept a pending transfer request.\n";

    std::cout << "\nInterractive commands will prompt for arguments selection and/or display "
        "choices to select from.\n";
    std::cout << "Some commands accept their arguments optionally and can be both interactive "
        "and non-interactive based on if all their arguments have been provided.\n";
    std::cout << "Command names and arguments are case-insensitive\n";

    std::cout << std::flush;
}

static auto operator<<(std::ostream &out, const FileShare::Config &config) -> std::ostream & {
    return out << "{\n" <<
        "\tDOWNLOAD_FOLDER = " << config.get_downloads_folder() << '\n' <<
        '}';
}

static auto operator<<(std::ostream &out, const FileShare::ServerConfig &config) -> std::ostream & {
    return out << "{\n" <<
        "\tDEVICE_UUID = " << config.get_uuid() << '\n' <<
        "\tDEVICE_NAME = " << config.get_device_name() << '\n' <<
        "\tPRIVATE_KEYS_DIR = " << config.get_private_keys_dir() << '\n' <<
        "\tPRIVATE_KEY_NAME = " << config.get_private_key_name() << '\n' <<
        '}';
}

static auto get_peer(InteractiveState &state, std::size_t index, bool include_pending = false, bool include_connected = true) -> FileShare::PeerBase_ptr {
    auto pending_peers = state.pending_peers;
    auto peers = state.peers;
    std::size_t total_peers = (include_pending ? pending_peers.size() : 0) + (include_connected ? peers.size() : 0);
    std::string_view peer_prefix = include_pending && !include_connected ? " pending" : (
        !include_pending && include_connected ? " connected" : ""
    );

    if (index <= pending_peers.size()) {
        auto peer = *std::next(pending_peers.begin(), static_cast<ssize_t>(index - 1));

        return peer;
    }
    if (index - pending_peers.size() <= peers.size()) {
        auto peer = *std::next(peers.begin(), static_cast<ssize_t>(index - pending_peers.size() - 1));

        return peer;
    }

    std::cerr << "Cannot find peer number " << index << " - there is only " << total_peers << peer_prefix << " peers\n";
    return nullptr;
}

static auto get_peer(InteractiveState &state, std::stringstream &ss, bool include_pending = false, bool include_connected = true) -> FileShare::PeerBase_ptr {
    std::size_t index = 0;

    if (!(ss >> index) || index == 0) {
        std::cerr << "Invalid Input ! Please enter a valid peer ID.\n";
    }
    return get_peer(state, index, include_pending, include_connected);
}

static auto get_peer(InteractiveState &state, const std::string &input, bool include_pending = false, bool include_connected = true) -> FileShare::PeerBase_ptr {
    std::stringstream ss{input};

    return get_peer(state, ss, include_pending, include_connected);
}

static auto get_peer(InteractiveState &state, std::string_view input, bool include_pending = false, bool include_connected = true) -> FileShare::PeerBase_ptr {
    // TODO: Find a better way to use string_view
    return get_peer(state, std::string(input), include_pending, include_connected);
}

// TODO: Check for extra args and fail
// TODO: Simplify this mess of copy-pasted commands

static void exit_command(InteractiveState &state, std::stringstream &args) {
    // TODO: Prompt if pending transfers
    server_run = false;
}

static void server_command(InteractiveState &state, std::stringstream &args) {
    FileShare::Utils::ci_string arg;
    bool current_state = !state.server.disabled();
    bool new_state;

    args >> arg;

    if (arg.empty()) {
        std::cout << "Server is currently " << (current_state ? "ON" : "OFF") << '\n';
        return;
    }

    if (arg == "ON") {
        new_state = true;
    } else if (arg == "OFF") {
        new_state = false;
    } else {
        std::cerr << "'" << arg << "' is not a valid server status. Please input either ON or OFF.\n";
        return;
    }

    if (new_state == current_state) {
        std::cout << "Server is already " << (current_state ? "ON" : "OFF") << '\n';
        return;
    }
    state.server.set_disabled(!new_state);
    std::cout << "Server is now " << (new_state ? "ON" : "OFF") << '\n';
}

// TODO: FIXME: Solve this mess of copy-pasted config commands with https://isocpp.org/wiki/faq/pointers-to-members#functionoids
// Idea is to create a Functionoid that takes a std::shared_ptr<T>, and specialise one of them for
// Config, and one for ServerConfig in the constructor. The interface of the functionoid would allow
// to call all the methods we need, and throw an error if the given method is not supported by that
// type of Config.
static void config_show_command(InteractiveState &state, std::string_view config, std::stringstream &args) {
    if (config == "server") {
        std::cout << state.server.get_config() << '\n';
    } else if (config == "default") {
        std::cout << state.server.get_peer_config() << '\n';
    } else {
        auto peer = std::dynamic_pointer_cast<FileShare::Peer>(get_peer(state, config));

        if (peer) {
            std::cout << peer->get_config() << '\n';
        }
    }
}

static void config_set_command(InteractiveState &state, std::string_view config, std::stringstream &args) {
    using ConfigSetter = std::function<FileShare::ServerConfig &(FileShare::ServerConfig *, std::string)>;
    using ConfigSetterMap = std::unordered_map<std::string, ConfigSetter>;
    static const ConfigSetterMap setter_map = {
        {"device_name", &FileShare::ServerConfig::set_device_name},
        {"private_keys_dir", &FileShare::ServerConfig::set_private_keys_dir},
        {"private_key_name", &FileShare::ServerConfig::set_private_key_name}
    };

    std::string name;
    std::string value;

    args >> name;
    args >> value; // TODO: Currently doesn't accept spaces in value. Look into std::quoted for that maybe ?

    string_lowercase(name);

    if (config == "server") {
        if (name == "device_uuid") {
            std::cerr << "DEVICE_UUID is read-only.\n";
            return;
        }
        auto fun = setter_map.find(name);

        if (fun == setter_map.end()) {
            std::cerr << "Unknown Config property " << std::quoted(name, '\'') << '\n';
            return;
        }
        (*fun).second(&state.server.get_config(), value);
        return state.server.restart();
    }

    if (name != "download_folder") {
        std::cerr << "Unkown Config property " << std::quoted(name, '\'') << '\n';
        return;
    }

    if (config == "default") {
        state.server.get_peer_config().set_downloads_folder(value);
    } else {
        auto peer = std::dynamic_pointer_cast<FileShare::Peer>(get_peer(state, config));

        if (peer) {
            peer->get_config().set_downloads_folder(value);
        }
    }
}

static void config_save_command(InteractiveState &state, std::string_view config, std::stringstream &args) {
    std::string path;

    args >> path;

    if (config == "server") {
        state.server.get_config().save(path);
    } else if (config == "default") {
        state.server.get_peer_config().save(path);
    } else {
        auto peer = std::dynamic_pointer_cast<FileShare::Peer>(get_peer(state, config));

        if (peer) {
            peer->get_config().save(path);
        }
    }
}

static void config_load_command(InteractiveState &state, std::string_view config, std::stringstream &args) {
    std::cerr << "TODO\n";
}

static void config_command(InteractiveState &state, std::stringstream &args) {
    static const ConfigCommandMap commands_map = {
        {"show", &config_show_command},
        {"set", &config_set_command},
        {"save", &config_save_command},
        {"load", &config_load_command}
    };

    std::string config;
    std::string subcommand;

    args >> config;
    args >> subcommand;

    string_lowercase(config);
    string_lowercase(subcommand);

    if (config.empty()) {
        std::cerr << "Please select the type of Config you would like to operate on.\n";
        return;
    }
    if (subcommand.empty()) {
        subcommand = "show";
    }
    auto fun = commands_map.find(subcommand);

    if (fun == commands_map.end()) {
        std::cerr << "Unknown Config Subcommand " << std::quoted(subcommand, '\'') << '\n';
        return;
    }
    return (*fun).second(state, config, args);
}

static void peer_list_command(InteractiveState &state, bool include_pending = false, bool include_connected = true) {
    auto peers = state.server.get_peers();
    auto pending_peers = state.server.get_pending_peers();

    std::size_t total_peers = (include_pending ? pending_peers.size() : 0) + (include_connected ? peers.size() : 0);
    std::size_t idx = 1;

    std::cout << "Currently connected Peers : " << total_peers << '\n';
    if (include_pending) {
        state.pending_peers.clear();
        state.pending_peers.reserve(pending_peers.size());
        for (const auto &peer : pending_peers) {
            // TODO: Display more stuff about peer
            std::cout << "\t" << idx << ". PENDING_ACCEPT - " << peer.second->get_device_uuid() << '\n';
            state.pending_peers.emplace_back(peer.second);
            idx++;
        }
    }

    if (include_connected) {
        state.peers.clear();
        state.peers.reserve(peers.size());
        for (const auto &peer : peers) {
            // TODO: Display more stuff about peer
            std::cout << "\t" << idx << ". - " << peer.second->get_device_uuid() << '\n';
            state.peers.emplace_back(peer.second);
            idx++;
        }
    }

    std::cout << std::flush;
}

static void select_peer(InteractiveState &state, bool include_pending = false, bool include_connected = true) {
    peer_list_command(state, include_pending, include_connected);
    std::cout << "Input the ID of the peer to select : " << std::flush;
    state.should_prompt = false;
}

static void peer_connect_command(InteractiveState &state, std::stringstream &args) {
    std::string ip_address;
    std::uint16_t port = 0;

    args >> ip_address;
    if (ip_address.empty()) {
        std::cerr << "Missing required argument <IP>\n";
        return;
    }
    if (args.eof()) {
        port = 12345;
    } else {
        args >> port;
    }
    if (args.fail() || port == 0) {
        std::cerr << "Please input a valid port number\n";
        return;
    }
    try {
        state.server.connect(CppSockets::EndpointV4(ip_address.c_str(), port));
    } catch (const std::runtime_error &e) {
        std::cerr << "Failed to connect to " << ip_address << ':' << port <<
            ". Error: " << e.what() << '\n';
    }
}

static void peer_accept_command(InteractiveState &state, std::stringstream &args) {
    auto pending_peers = state.server.get_pending_peers();

    if (args.eof()) {
        state.state = InteractiveStateMachine::ACCEPT_MENU;
        peer_list_command(state, true, false);
        return;
    }
    auto peer = std::dynamic_pointer_cast<FileShare::PreAuthPeer>(get_peer(state, args, true, false));

    if (peer) {
        state.server.accept_peer(peer);
    }
}

static void peer_disconnect_command(InteractiveState &state, std::stringstream &args) {
    if (args.eof()) {
        state.state = InteractiveStateMachine::DISCONNECT_MENU;
        peer_list_command(state, true);
        return;
    }
    auto peer = get_peer(state, args, true, false);

    if (peer) {
        peer->disconnect();
    }
}

static void peer_select_command(InteractiveState &state, std::stringstream &args) {
    peer_list_command(state);

    // TODO: Need to disable it if it disconnects -> We need events for that too

    // TODO: Activate Peer ID (De-Activate current peer if already selected.)
    // Also displays currently active peer if any, and mentions the De-Activation feature in prompt
}

static void peer_command(InteractiveState &state, std::stringstream &args) {
    static const CommandMap commands_map = {
        {"list", [](InteractiveState &state, std::stringstream &){ peer_list_command(state, true); }},
        {"connect", &peer_connect_command},
        {"disconnect", &peer_disconnect_command},
        {"accept", &peer_accept_command},
        {"select", &peer_select_command}
    };

    std::string subcommand;

    args >> subcommand;
    string_lowercase(subcommand);

    auto fun = commands_map.find(subcommand);

    if (fun == commands_map.end()) {
        std::cerr << "Unknown Peer Subcommand " << std::quoted(subcommand, '\'') << '\n';
        return;
    }
    return (*fun).second(state, args);
}

static void execute_list_files_command(FileShare::Peer_ptr &selected_peer, InteractiveState &state, std::stringstream &args) {
    FileShare::Protocol::Response<std::vector<FileShare::Protocol::FileInfo>> result;
    std::string path;

    args >> path;
    result = selected_peer->list_files(path);
    std::cout << "Listing files in " << std::quoted(path) << " - got " << result.code << ". " <<
        result.response->size() << " Files : \n";
    for (const auto &file : *result.response) {
        std::cout << '\t' << file.file_type << ": " << file.path << '\n';
    }
}

static void execute_receive_file_command(FileShare::Peer_ptr &selected_peer, InteractiveState &state, std::stringstream &args) {
    std::string path;
    FileShare::Protocol::Response<void> result;

    args >> path;
    if (path.empty()) {
        std::cerr << "The filepath of the file to receive is required\n";
        return;
    }

    result = selected_peer->receive_file(path);
    std::cout << "Receive File status: " << result.code << '\n';
}

static void execute_send_file_command(FileShare::Peer_ptr &selected_peer, InteractiveState &state, std::stringstream &args) {
    std::string path;
    FileShare::Protocol::Response<void> result;

    args >> std::quoted(path);
    if (path.empty()) {
        std::cerr << "The filepath of the file to send is required\n";
        return;
    }
    path = FileShare::Utils::resolve_home_component(path);

    result = selected_peer->send_file(path);
    std::cout << "Send File status: " << result.code << '\n';
}

static auto get_execute_subcommand(const std::string &subcommand) -> std::optional<ExecuteCommandMap::value_type> {
    static const ExecuteCommandMap commands_map = {
        {"list_files", &execute_list_files_command},
        {"receive_file", &execute_receive_file_command},
        {"send_file", &execute_send_file_command}
    };

    auto fun = commands_map.find(subcommand);

    if (fun == commands_map.end()) {
        std::cerr << "Unknown Execute Subcommand " << std::quoted(subcommand, '\'') << '\n';
        return {};
    }

    return *fun;
}

static void execute_command(InteractiveState &state, std::stringstream &args) {
    std::string subcommand;

    args >> subcommand;
    string_lowercase(subcommand);

    auto fun = get_execute_subcommand(subcommand);
    if (!fun.has_value()) {
        return;
    }

    auto selected_peer = state.selected_peer.lock();

    if (!selected_peer) {
        auto peers = state.server.get_peers();

        if (peers.size() > 1) {
            state.state = InteractiveStateMachine::EXECUTE_MENU;
            state.sub_menu_state = SubMenuCommandState{.subcommand = subcommand, .args = std::move(args)};
            std::cout << "Multiple peers are currently connected. Please select a peer to execute the command on :\n";
            select_peer(state);
            return;
        }
        if (peers.size() == 1) {
            selected_peer = peers.begin()->second;
        } else {
            std::cerr << "No Connected Peers\n";
            return;
        }
    }
    return (*fun).second(selected_peer, state, args);
}

static void transfers_command(InteractiveState &state, std::stringstream &args) {
    // TODO: List Transfers
}

static void handle_main_menu(InteractiveState &state, const std::string &input) {
    static const CommandMap commands_map = {
        {"help", [](InteractiveState &, std::istream &){ display_interactive_help(); }},
        {"exit", &exit_command},
        {"server", &server_command},
        {"config", &config_command},
        {"peer", &peer_command},
        {"execute", &execute_command},
        {"transfers", &transfers_command}
    };

    std::stringstream ss(input);
    std::string cmd;

    ss >> cmd;
    string_lowercase(cmd);

    if (cmd == "?") {
        cmd = "help";
    } else if (cmd == "quit") {
        cmd = "exit";
    }

    auto fun = commands_map.find(cmd);

    if (fun == commands_map.end()) {
        std::cerr << "Unknown Command " << std::quoted(input, '\'') << '\n';
        std::cerr << "Type HELP or '?' for help.\n";
        return;
    }

    return (*fun).second(state, ss);
}

// TODO: Test this
static void handle_execute_menu(InteractiveState &state, const std::string &input) {
    auto peer = std::dynamic_pointer_cast<FileShare::Peer>(get_peer(state, input));

    if (peer) {
        auto fun = get_execute_subcommand(state.sub_menu_state.subcommand);

        (*fun).second(peer, state, state.sub_menu_state.args);
        state.sub_menu_state = {};
        state.state = InteractiveStateMachine::MAIN_MENU;
    }
}

static void handle_interactive_input(InteractiveState &state, const std::string &input) {
    switch (state.state) {
        case InteractiveStateMachine::MAIN_MENU:
            return handle_main_menu(state, input);
        case InteractiveStateMachine::EXECUTE_MENU:
            return handle_execute_menu(state, input);
        case InteractiveStateMachine::EXIT_MENU:
        case InteractiveStateMachine::CONFIG_MENU:
        case InteractiveStateMachine::DISCONNECT_MENU:
        case InteractiveStateMachine::ACCEPT_MENU:
        case InteractiveStateMachine::SELECT_MENU:
        case InteractiveStateMachine::TRANSFERS_MENU:
            std::cout << "TODO - OTHER STATES\n";
            state.state = InteractiveStateMachine::MAIN_MENU;
            return;
    }
}

// TODO: Pretty Colors
static void display_prompt(InteractiveState &state) {
    if (!state.should_prompt) {
        return;
    }
    std::stringstream ss;
    bool empty = true;

    std::size_t pending_requests = state.pending_events.size();
    std::size_t pending_peers = state.server.get_pending_peers().size();

    // TODO: Display any event that happened since last prompt (eg: Peer Disconnected, New Requests)
    if (pending_peers > 0) {
        ss << "Pending Connections: " << pending_peers;
        empty = false;
    }

    if (pending_requests > 0) {
        ss << (empty ? "" : ", ") << "Pending Requests: " << pending_requests;
        empty = false;

        std::cout << "TODO: REMOVE - Accepting all pending requests\n";
        for (auto &event : state.pending_events) {
            if (event.REQUEST) {
                auto peer = std::dynamic_pointer_cast<FileShare::Peer>(event.peer());

                peer->respond_to_request(event.request().value(), FileShare::Protocol::StatusCode::STATUS_OK);
            }
        }
        state.pending_events.clear();
    }
    std::cout << ss.str() << (empty ? "" : ".\n");

    std::cout << "(fs-cli) ";
    if (state.state != InteractiveStateMachine::MAIN_MENU) {
        std::cout << state.state << " ";
    }
    std::cout << "> " << std::flush;
    state.should_prompt = false;
}

void interactive_mode(FileShare::Server &server) {
    InteractiveState state(server);

    FileShare::Server::PeerAcceptCallback accept_cb = [&](FileShare::Server &, FileShare::PreAuthPeer_ptr &) {
        return false; // We will handle acceptation ourselves
    };
    FileShare::Server::PeerRequestCallback request_cb = [&](FileShare::Server &, FileShare::Peer_ptr &client, FileShare::Protocol::Request &request) {
        state.pending_events.emplace_back(FileShare::Server::Event::REQUEST, client, request);
    };
    std::array<char, 256> read_buff = {0};
    struct timespec timeout = {.tv_sec = 1, .tv_nsec = 0}; // 1s
    std::vector<struct pollfd> fds;

    fds.emplace_back(pollfd({.fd = server.get_socket().get_fd(), .events = POLLIN, .revents = 0}));
    struct pollfd *stdin_fd = &fds.emplace_back(pollfd({.fd = STDIN_FILENO, .events = POLLIN, .revents = 0}));
    int nb_ready = 0;

    // Can return old sig_handler or SIG_ERR on failure, but we dont care either way
    (void)std::signal(SIGINT, signal_handler);

    std::cout << "Welcome to the FileShare CLI !" << "\n\n";
    // TODO: Find more stuff to say on startup
    // TODO: Would be nice if `?` works without \n
    std::cout << "Enter Command (type HELP or '?' for help): " << "\n";

    server_run = true;
    while (server_run) {
        display_prompt(state);

        // TODO: Avoid having 2 polls (here + server)
        nb_ready = FileShare::Utils::poll(fds, &timeout);

        if (nb_ready == 0) {
            continue; // Timeout, TODO: Refresh screen if needed
        }
        // if (nb_ready < 0) {
        //     // TODO handle Signals or Regular Errors
        // }
        if (stdin_fd->revents & (POLLIN | POLLHUP)) {
            nb_ready--;
            if (stdin_fd->revents & (POLLIN)) {
                std::size_t nb = ::read(STDIN_FILENO, read_buff.data(), read_buff.size());
                std::string buffer = std::string(read_buff.data(), nb);

                if (buffer.empty()) {  // CTRL+D -> Quitting
                    buffer = "EXIT";
                    std::cout << buffer;
                }

                state.should_prompt = true;
                if (buffer.find('\n') == std::string::npos) {
                    std::cout << '\n';
                }
                std::stringstream ss(buffer);
                do {
                    std::string input;

                    std::getline(ss, input);
                    if (!input.empty()) {
                        handle_interactive_input(state, input);
                    }
                } while(ss);
            } else {
                std::cout << "PULLUP -> Quitting...\n";
                break;
            }
        }
        if (nb_ready > 0) {
            server.process_events(accept_cb, request_cb); // Server poll + handle events

            // TODO: Smth happened on server -> \r and refresh screen
        }

        // Peers could have connected / disconnected, need to refresh FDs
        // TODO: Find a better way.
        // Currently outside of if (nb_ready) because we need to run it after PEER CONNECT
        fds.clear();
        fds.reserve(server.get_poll_fds().size() + 1);
        for (auto poll_fd : server.get_poll_fds()) {
            fds.emplace_back(poll_fd);
        }
        stdin_fd = &fds.emplace_back(pollfd({.fd = STDIN_FILENO, .events = POLLIN, .revents = 0}));
    }
}

// TODO: Use ncurses (And PDCurses ?) to remove line-buffering
auto interactive_user_question(const std::string_view &question) -> bool {
    std::string input;

    do {
        std::cout << question << " (y/n) " << std::flush;
        std::getline(std::cin, input);

        string_trim(input);

        if (input == "y" || input == "yes") {
            return true;
        }

        // TODO: Handle EOF differently
        if (input == "n" || input == "no" || std::cin.eof()) {
            return false;
        }

        std::cout << "\nPlease answer with y/Y or n/N.\n";
    } while (true);
}

auto interactive_user_input(const std::string_view &question) -> std::string {
    std::string input;

    do {
        std::cout << question << ": " << std::flush;
        std::getline(std::cin, input);

        string_trim(input);
        if (!input.empty() || !std::cin) { // EOF or error
            break;
        }

        std::cout << "Input cannot be empty. Please try again.\n";
    } while (true);

    return input;
}
