/*
** Project Project FileShare, 2025
**
** Author Francois Michaut
**
** Started on  Wed Jul 23 13:49:52 2025 Francois Michaut
** Last update Sun Aug 24 19:57:35 2025 Francois Michaut
**
** interactive.cpp : Interractive Mode logic
*/

#include "FileShareVersion.hpp"

#include <FileShare/Config/ServerConfig.hpp>
#include <FileShare/Protocol/Definitions.hpp>
#include <FileShare/Server.hpp>
#include <FileShare/Utils/Poll.hpp>
#include <FileShare/Utils/Strings.hpp>
#include <FileShare/Utils/Path.hpp>

#include <CppSockets/IPv4.hpp>

#include <algorithm>
#include <charconv>
#include <csignal>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <system_error>
#include <unistd.h>

extern bool server_run;
void signal_handler(int sig);

enum class InteractiveStateMachine {
    MAIN_MENU       = 0x00,
    EXIT_MENU       = 0x01,
    CONFIG_MENU     = 0x05,

    DISCONNECT_MENU = 0x12,
    ACCEPT_MENU     = 0x13,
    SELECT_MENU     = 0x16,

    EXECUTE_MENU    = 0x20,

    TRANSFERS_MENU  = 0x30,
};

auto operator<<(std::ostream &out, InteractiveStateMachine state) -> std::ostream & {
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

struct InteractiveState {
    InteractiveState(FileShare::Server &server) : server(server) {}

    FileShare::Server &server;
    std::vector<FileShare::Server::Event> pending_events;
    std::weak_ptr<FileShare::Peer> selected_peer;

    InteractiveStateMachine state = InteractiveStateMachine::MAIN_MENU;
};

using CommandMap = std::unordered_map<std::string, std::function<void(InteractiveState &, std::istream &)>>;
using ConfigCommandMap = std::unordered_map<std::string, std::function<void(InteractiveState &, std::string_view, std::istream &)>>;

static void display_interactive_help() {
    std::cout << "Available Commands are :" << '\n';
    std::cout << "\t" << "- HELP: Display this help message." << '\n';
    std::cout << "\t" << "- EXIT|QUIT: Stop the program." << '\n';
    std::cout << "\t" << "- SERVER (ON|OFF): Toggle the Server ON or OFF.\n\t\t If Server is OFF, "
        "no external peers can connect, but you can still initiate connections (existing "
        "connections will not be terminated)." << '\n';

    std::cout << "\t" << "- CONFIG (SERVER|DEFAULT|<PEER_ID>) [SUBCOMMAND] [<ARG>]...: "
        "(Interractive) Modify configuration on the fly. Available subcommands :" << '\n';
    std::cout << "\t\t" << "- SHOW: (default) Display the selected configuration." << '\n';
    std::cout << "\t\t" << "- SET <NAME> <VALUE>: Set a simple configuration value." << '\n';
    std::cout << "\t\t" << "- SAVE [<PATH>]: Save the config to a file at PATH. "
        "Overwrites file if exists. If PATH is empty, will save into the original file." << '\n';
    std::cout << "\t\t" << "- LOAD [<PATH>]: Replace the current config with the one in the "
        "config file at PATH. If PATH is empty, will load from the default location." << '\n';
    // TODO: FileMap config support

    std::cout << "\t" << "- PEER <SUBCOMMAND> [<ARG>]...: Manage peers. Available subcommands :"
              << '\n';
    std::cout << "\t\t" << "- LIST: List currently connected peers." << '\n';
    std::cout << "\t\t" << "- CONNECT <IP> [<PORT>]: Connect to a new peer." << '\n';
    std::cout << "\t\t" << "- DISCONNECT [<ID>]: (Interractive) Disconnect from a connected peer "
        "or reject a connection request." << '\n';
    std::cout << "\t\t" << "- ACCEPT [<ID>]: (Interractive) Accept a connection request from an "
        "external peer." << '\n';
    std::cout << "\t\t" << "- SELECT [<ID>]: (Interractive) Promote a peer as the 'selected' one."
        "\n\t\t\t If connected to multiple peers, pre-selecting a peer allows to send multiple "
        "commands without having to select it each time." << '\n';

    // TODO: Where do I select Peer if multiple and None selected ?
    std::cout << "\t" << "- EXECUTE <COMMAND> [<ARG>]...: (Interractive) Execute a command on a "
        "selected peer. Will prompt to select a peer if none selected. Available commands :"
              << '\n';
    std::cout << "\t\t" << "- LIST_FILES [<PATH>]: List availables files on the peer. Can be "
        "Downloaded using RECEIVE_FILE." << '\n';
    std::cout << "\t\t" << "- RECEIVE_FILE <REMOTE_PATH>: Request a file download" << '\n';
    std::cout << "\t\t" << "- SEND_FILE <LOCAL_PATH>: Send a file to the peer." << '\n';

    std::cout << "\t" << "- TRANSFERS: Display ongoing Downloads and Uploads transfers and their "
        "progress." << '\n';

    std::cout << "\n" << "Interractive commands will prompt for arguments selection or subsequent "
        "required inputs and/or display choices to select from." << '\n';
    std::cout << "Some commands accept their arguments optionally and can be both interactive "
        "and non-interactive based on if all their arguments have been provided." << '\n';
    std::cout << "Command names and arguments are case-insensitive" << '\n';

    std::cout << std::flush;
}

void lowercase(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char chr){ return std::tolower(chr); });
}

auto operator<<(std::ostream &out, const FileShare::Config &config) -> std::ostream & {
    return out << '{' << '\n' <<
        '\t' << "DOWNLOAD_FOLDER = " << config.get_downloads_folder() << '\n' <<
        '}';
}

auto operator<<(std::ostream &out, const FileShare::ServerConfig &config) -> std::ostream & {
    return out << '{' << '\n' <<
        '\t' << "DEVICE_UUID = " << config.get_uuid() << '\n' <<
        '\t' << "DEVICE_NAME = " << config.get_device_name() << '\n' <<
        '\t' << "PRIVATE_KEYS_DIR = " << config.get_private_keys_dir() << '\n' <<
        '\t' << "PRIVATE_KEY_NAME = " << config.get_private_key_name() << '\n' <<
        '}';
}

static auto get_peer(InteractiveState &state, std::size_t index, bool include_pending = false, bool include_connected = true) -> std::shared_ptr<FileShare::PeerBase> {
    auto pending_peers = state.server.get_pending_peers();
    auto peers = state.server.get_peers();
    std::size_t total_peers = (include_pending ? pending_peers.size() : 0) + (include_connected ? peers.size() : 0);
    std::string_view peer_prefix = include_pending && !include_connected ? " pending" : (
        !include_pending && include_connected ? " connected" : ""
    );

    if (index <= pending_peers.size()) {
        auto peer = *std::next(pending_peers.begin(), static_cast<ssize_t>(index - 1));

        return peer.second;
    }
    if (index - pending_peers.size() <= peers.size()) {
        auto peer = *std::next(peers.begin(), static_cast<ssize_t>(index - pending_peers.size() - 1));

        return peer.second;
    }

    std::cerr << "Cannot find peer number " << index << " - there is only " << total_peers << peer_prefix << " peers" << std::endl;
    return nullptr;
}

// TODO: Check for extra args and fail
// TODO: Simplify this mess of copy-pasted commands

static void exit_command(InteractiveState &state, std::istream &args) {
    // TODO: Prompt if pending transfers
    server_run = false;
}

static void server_command(InteractiveState &state, std::istream &args) {
    FileShare::Utils::ci_string arg;
    bool current_state = !state.server.disabled();
    bool new_state;

    args >> arg;

    if (arg.empty()) {
        std::cout << "Server is currently " << (current_state ? "ON" : "OFF") << std::endl;
        return;
    }

    if (arg == "ON") {
        new_state = true;
    } else if (arg == "OFF") {
        new_state = false;
    } else {
        std::cerr << "'" << arg << "' is not a valid server status. Please input either ON or OFF." << std::endl;
        return;
    }

    if (new_state == current_state) {
        std::cout << "Server is already " << (current_state ? "ON" : "OFF") << std::endl;
        return;
    }
    state.server.set_disabled(!new_state);
    std::cout << "Server is now " << (new_state ? "ON" : "OFF") << std::endl;
}

// TODO: FIXME: Solve this mess of copy-pasted config commands with https://isocpp.org/wiki/faq/pointers-to-members#functionoids
// Idea is to create a Functionoid that takes a std::shared_ptr<T>, and specialise one of them for
// Config, and one for ServerConfig in the constructor. The interface of the functionoid would allow
// to call all the methods we need, and throw an error if the given method is not supported by that
// type of Config.
static void config_show_command(InteractiveState &state, std::string_view config, std::istream &args) {
    if (config == "server") {
        std::cout << state.server.get_config() << std::endl;
    } else if (config == "default") {
        std::cout << state.server.get_peer_config() << std::endl;
    } else {
        std::size_t index = 0;
        auto result = std::from_chars(config.data(), config.data() + config.size(), index);

        if (result.ec == std::errc()) {
            auto peer = std::dynamic_pointer_cast<FileShare::Peer>(get_peer(state, index));

            if (peer) {
                std::cout << peer->get_config() << std::endl;
            }
        } else {
            std::cerr << "Please input a valid Peer ID" << std::endl;
        }
    }
}

static void config_set_command(InteractiveState &state, std::string_view config, std::istream &args) {
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

    lowercase(name);

    if (config == "server") {
        if (name == "device_uuid") {
            std::cerr << "DEVICE_UUID is read-only." << std::endl;
            return;
        }
        auto fun = setter_map.find(name);

        if (fun == setter_map.end()) {
            std::cerr << "Unknown Config property " << std::quoted(name, '\'') << std::endl;
            return;
        }
        (*fun).second(&state.server.get_config(), value);
        return state.server.restart();
    }

    if (name != "download_folder") {
        std::cerr << "Unkown Config property " << std::quoted(name, '\'') << std::endl;
        return;
    }

    if (config == "default") {
        state.server.get_peer_config().set_downloads_folder(value);
    } else {
        std::size_t index = 0;
        auto result = std::from_chars(config.data(), config.data() + config.size(), index);

        if (result.ec == std::errc()) {
            auto peer = std::dynamic_pointer_cast<FileShare::Peer>(get_peer(state, index));

            if (peer) {
                peer->get_config().set_downloads_folder(value);
            }
        } else {
            std::cerr << "Please input a valid Peer ID" << std::endl;
        }
    }
}

static void config_save_command(InteractiveState &state, std::string_view config, std::istream &args) {
    std::string path;

    args >> path;

    if (config == "server") {
        state.server.get_config().save(path);
    } else if (config == "default") {
        state.server.get_peer_config().save(path);
    } else {
        std::size_t index = 0;
        auto result = std::from_chars(config.data(), config.data() + config.size(), index);

        if (result.ec == std::errc()) {
            auto peer = std::dynamic_pointer_cast<FileShare::Peer>(get_peer(state, index));

            if (peer) {
                peer->get_config().save(path);
            }
        } else {
            std::cerr << "Please input a valid Peer ID" << std::endl;
        }
    }

}

static void config_load_command(InteractiveState &state, std::string_view config, std::istream &args) {
    std::cerr << "TODO" << std::endl;
}

static void config_command(InteractiveState &state, std::istream &args) {
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

    lowercase(config);
    lowercase(subcommand);

    if (config.empty()) {
        std::cerr << "Please select the type of Config you would like to operate on." << std::endl;
        return;
    }
    if (subcommand.empty()) {
        subcommand = "show";
    }
    auto fun = commands_map.find(subcommand);

    if (fun == commands_map.end()) {
        std::cerr << "Unknown Config Subcommand " << std::quoted(subcommand, '\'') << std::endl;
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
        for (const auto &peer : pending_peers) {
            // TODO: Display more stuff about peer
            std::cout << "\t" << idx << ". PENDING_ACCEPT - " << peer.second->get_device_uuid() << '\n';
            idx++;
        }
    }

    if (include_connected) {
        for (const auto &peer : peers) {
            // TODO: Display more stuff about peer
            std::cout << "\t" << idx << ". - " << peer.second->get_device_uuid() << '\n';
            idx++;
        }
    }

    std::cout << std::flush;
}

static void peer_connect_command(InteractiveState &state, std::istream &args) {
    std::string ip;
    std::uint16_t port = 0;

    args >> ip;
    if (ip.empty()) {
        std::cerr << "Missing required argument <IP>" << std::endl;
        return;
    }
    if (!args.eof()) {
        args >> std::ws;
    }
    if (args.eof()) {
        port = 12345;
    } else {
        args >> port;
    }
    if (args.fail() || port == 0) {
        std::cerr << "Please input a valid port number" << std::endl;
        return;
    }
    state.server.connect(CppSockets::EndpointV4(ip.c_str(), port));
}

static void peer_accept_command(InteractiveState &state, std::istream &args) {
    std::size_t index = 0;
    auto pending_peers = state.server.get_pending_peers();

    if (!args.eof()) {
        args >> std::ws;
    }
    if (args.eof()) {
        state.state = InteractiveStateMachine::ACCEPT_MENU;
        peer_list_command(state, true, false);
        return;
    }
    args >> index;
    if (args.fail() || index == 0) {
        std::cerr << "Please input a valid number" << std::endl;
        return;
    }
    auto peer = std::dynamic_pointer_cast<FileShare::PreAuthPeer>(get_peer(state, index, true, false));

    if (peer) {
        state.server.accept_peer(peer);
    }
}

static void peer_disconnect_command(InteractiveState &state, std::istream &args) {
    std::size_t index = 0;

    if (!args.eof()) {
        args >> std::ws;
    }
    if (args.eof()) {
        state.state = InteractiveStateMachine::DISCONNECT_MENU;
        peer_list_command(state, true);
        return;
    }
    args >> index;
    if (args.fail() || index == 0) {
        std::cerr << "Please input a valid number" << std::endl;
        return;
    }
    auto peer = get_peer(state, index, true, false);

    if (peer) {
        peer->disconnect();
    }
}

static void peer_select_command(InteractiveState &state, std::istream &args) {
    peer_list_command(state);

    // TODO: Need to disable it if it disconnects -> We need events for that too

    // TODO: Activate Peer ID (De-Activate current peer if executed twice.)
    // Also displays currently active peer if any, and mentions the De-Activation feature in prompt
}

static void peer_command(InteractiveState &state, std::istream &args) {
    static const CommandMap commands_map = {
        {"list", [](InteractiveState &state, std::istream &){ peer_list_command(state, true); }},
        {"connect", &peer_connect_command},
        {"disconnect", &peer_disconnect_command},
        {"accept", &peer_accept_command},
        {"select", &peer_select_command}
    };

    std::string subcommand;

    args >> subcommand;
    lowercase(subcommand);

    auto fun = commands_map.find(subcommand);

    if (fun == commands_map.end()) {
        std::cerr << "Unknown Peer Subcommand " << std::quoted(subcommand, '\'') << std::endl;
        return;
    }
    return (*fun).second(state, args);
}

static void execute_list_files_command(InteractiveState &state, std::istream &args) {
    auto selected_peer = state.selected_peer.lock();
    FileShare::Protocol::Response<std::vector<FileShare::Protocol::FileInfo>> result;
    std::string path;

    args >> path;
    if (selected_peer) {
        result = selected_peer->list_files(path);
    } else {
        auto peers = state.server.get_peers();

        if (peers.size() > 1) {
            state.state = InteractiveStateMachine::EXECUTE_MENU;
            // TODO: Store command state (args) to be re-executed when we get the peer
            return;
        }
        if (peers.size() == 1) {
            result = peers.begin()->second->list_files(path);
        } else {
            std::cerr << "No Connected Peers" << std::endl;
            return;
        }
    }
    std::cout << "Listing files in " << std::quoted(path) << " - got " << result.code << ". " <<
        result.response->size() << " Files : " << '\n';
    for (const auto &file : *result.response) {
        std::cout << '\t' << file.file_type << ": " << file.path << '\n';
    }
}

static void execute_receive_file_command(InteractiveState &state, std::istream &args) {
    auto selected_peer = state.selected_peer.lock();
    std::string path;
    FileShare::Protocol::Response<void> result;

    args >> path;
    if (path.empty()) {
        std::cerr << "The filepath of the file to receive is required" << std::endl;
        return;
    }

    if (selected_peer) {
        result = selected_peer->receive_file(path);
    } else {
        auto peers = state.server.get_peers();

        if (peers.size() > 1) {
            state.state = InteractiveStateMachine::EXECUTE_MENU;
            // TODO: Store command state (args) to be re-executed when we get the peer
            return;
        }
        if (peers.size() == 1) {
            result = peers.begin()->second->receive_file(path);
        } else {
            std::cerr << "No Connected Peers" << std::endl;
            return;
        }
    }
    std::cout << "Receive File status: " << result.code << std::endl;
}

static void execute_send_file_command(InteractiveState &state, std::istream &args) {
    auto selected_peer = state.selected_peer.lock();
    std::string path;
    FileShare::Protocol::Response<void> result;

    args >> path;
    if (path.empty()) {
        std::cerr << "The filepath of the file to send is required" << std::endl;
        return;
    }
    path = FileShare::Utils::resolve_home_component(path);

    if (selected_peer) {
        result = selected_peer->send_file(path);
    } else {
        auto peers = state.server.get_peers();

        if (peers.size() > 1) {
            state.state = InteractiveStateMachine::EXECUTE_MENU;
            // TODO: Store command state (args) to be re-executed when we get the peer
            return;
        }
        if (peers.size() == 1) {
            result = peers.begin()->second->send_file(path);
        } else {
            std::cerr << "No Connected Peers" << std::endl;
            return;
        }
    }
    std::cout << "Send File status: " << result.code << std::endl;
}

static void execute_command(InteractiveState &state, std::istream &args) {
    // TODO: Execute CMD ARG1 [ARG2...]; If multiple clients connected, will need to prompt for select.
    static const CommandMap commands_map = {
        {"list_files", &execute_list_files_command},
        {"receive_file", &execute_receive_file_command},
        {"send_file", &execute_send_file_command}
    };

    std::string subcommand;

    args >> subcommand;
    lowercase(subcommand);

    auto fun = commands_map.find(subcommand);

    if (fun == commands_map.end()) {
        std::cerr << "Unknown Execute Subcommand " << std::quoted(subcommand, '\'') << std::endl;
        return;
    }
    return (*fun).second(state, args);
}

static void transfers_command(InteractiveState &state, std::istream &args) {
    // TODO: List Transfers
}

static void handle_main_menu(InteractiveState &state, const std::string &input) {
    static const CommandMap commands_map = {
        {"help", [](InteractiveState &, std::istream &){ display_interactive_help(); }},
        {"exit", &exit_command},
        {"quit", &exit_command},
        {"server", &server_command},
        {"config", &config_command},
        {"peer", &peer_command},
        {"execute", &execute_command},
        {"transfers", &transfers_command}
    };

    std::stringstream ss(input);
    std::string cmd;

    ss >> cmd;
    lowercase(cmd);

    if (cmd == "?") {
        cmd = "help";
    }

    auto fun = commands_map.find(cmd);

    if (fun == commands_map.end()) {
        std::cerr << "Unknown Command " << std::quoted(input, '\'') << std::endl;
        std::cerr << "Type HELP or '?' for help." << std::endl;
        return;
    }

    return (*fun).second(state, ss);
}

static void handle_interactive_input(InteractiveState &state, const std::string &input) {
    switch (state.state) {
        case InteractiveStateMachine::MAIN_MENU:
            return handle_main_menu(state, input);
        case InteractiveStateMachine::EXIT_MENU:
        case InteractiveStateMachine::CONFIG_MENU:
        case InteractiveStateMachine::DISCONNECT_MENU:
        case InteractiveStateMachine::ACCEPT_MENU:
        case InteractiveStateMachine::SELECT_MENU:
        case InteractiveStateMachine::EXECUTE_MENU:
        case InteractiveStateMachine::TRANSFERS_MENU:
            std::cout << "TODO - OTHER STATES" << std::endl;
            state.state = InteractiveStateMachine::MAIN_MENU;
            return;
    }
}

// TODO: Pretty Colors
static void display_prompt(InteractiveState &state, bool &should_prompt) {
    if (!should_prompt) {
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

        std::cout << "TODO: REMOVE - Accepting all pending requests" << std::endl;
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
    should_prompt = false;
}

void interactive_mode(FileShare::Server &server) {
    InteractiveState state(server);

    FileShare::Server::PeerAcceptCallback accept_cb = [&](FileShare::Server &, std::shared_ptr<FileShare::PreAuthPeer> &) {
        return false; // We will handle acceptation ourselves
    };
    FileShare::Server::PeerRequestCallback request_cb = [&](FileShare::Server &, std::shared_ptr<FileShare::Peer> &client, FileShare::Protocol::Request &request) {
        state.pending_events.emplace_back(FileShare::Server::Event::REQUEST, client, request);
    };
    std::array<char, 256> read_buff = {0};
    struct timespec timeout = {1, 0}; // 1s
    std::vector<struct pollfd> fds;

    fds.emplace_back(pollfd({server.get_socket().get_fd(), POLLIN, 0}));
    struct pollfd *stdin_fd = &fds.emplace_back(pollfd({STDIN_FILENO, POLLIN, 0}));
    bool should_prompt = true;
    int nb_ready = 0;

    std::signal(SIGINT, signal_handler);

    std::cout << "Welcome to the FileShare CLI !" << "\n\n";
    // TODO: Find more stuff to say on startup
    // TODO: Would be nice if `?` works without \n
    std::cout << "Enter Command (type HELP or '?' for help): " << "\n";

    server_run = true;
    while (server_run) {
        display_prompt(state, should_prompt);

        // TODO: Avoid having 2 polls (here + server)
        nb_ready = FileShare::Utils::poll(fds, &timeout);

        if (nb_ready == 0) {
            continue;
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

                should_prompt = true;
                if (buffer.find('\n') == std::string::npos) {
                    std::cout << std::endl;
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
                std::cout << "PULLUP -> Quitting..." << std::endl;
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
