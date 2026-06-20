/*
** Project FileShare, 2023
**
** Author Francois Michaut
**
** Started on  Sat Nov 11 11:06:03 2023 Francois Michaut
** Last update Sat May 23 17:24:04 2026 Francois Michaut
**
** main.cpp : Main entry point of FileShare CLI
*/

#include "FileShareVersion.hpp"

#include <FileShare/Config/ServerConfig.hpp>
#include <FileShare/Errors/ConfigErrors.hpp>
#include <FileShare/Peer/PreAuthPeer.hpp>
#include <FileShare/Server.hpp>

#include <argparse/argparse.hpp>

#include <csignal>
#include <iostream>
#include <memory>

// TODO: Check if we can use _CRT_NONSTDC_NO_DEPRECATE under windows
#include <unistd.h>

#define DESCRIPTION "" // TODO

#define SERVER_ARG "--server"
#define CONNECT_ARG "--connect"
#define INTERACTIVE_ARG "--interactive"
#define EXECUTE_ARG "--execute"
#define CONFIG_ARG "--config"
#define SERVER_CONFIG_ARG "--server-config"
#define DEFAULT_CONFIG_ARG "--default-config"
#define DEFAULT_SERVER_CONFIG_ARG "--default-server-config"

void interactive_mode(FileShare::Server &server);

auto interactive_user_question(const std::string_view &question) -> bool;
auto interactive_user_input(const std::string_view &question) -> std::string;

static auto setup_args() -> std::shared_ptr<argparse::ArgumentParser> {
    auto parser = std::make_shared<argparse::ArgumentParser>("file-share-cli", FILE_SHARE_VERSION);

    parser->add_description(DESCRIPTION);

    // TODO: Add argument groups, and group arguments better

    parser->add_argument("-s", SERVER_ARG)
        .flag()
        .help("Enable server, allowing other peers to connect. The program will keep running until killed.");

    parser->add_argument(SERVER_CONFIG_ARG)
        .metavar("PATH")
        .help("Speficy the path to the server config file.");

    parser->add_argument(CONFIG_ARG)
        .metavar("PATH")
        .help("Speficy the path to the peer config file.");

    parser->add_argument(DEFAULT_SERVER_CONFIG_ARG)
        .flag()
        .help("Use the default server config.");

    parser->add_argument(DEFAULT_CONFIG_ARG)
        .flag()
        .help("Use the default peer config.");

    parser->add_argument("-c", CONNECT_ARG)
        .nargs(1, 2) // TODO: Instead of using nargs, use a custom action() that would parse the IP and Port in an IPv4 Endpoint. Also change format to ip[:port]
        .metavar("IP [PORT]") // TODO: Remove the extra "..." that argparse adds
        .help("Connect to an external server. Format: '-c ip [port]'. Required to execute commands.");

    parser->add_argument("-e", EXECUTE_ARG)
        .append()
        .metavar("COMMAND")
        .help("Add a command to be executed. Repeat the flag to add multiple commands.");

    parser->add_argument("-i", INTERACTIVE_ARG)
        .flag()
        .help("Enable interactive mode. You will be able to change settings and execute commands from a CLI.");

    // parser->add_epilog(); // TODO ?

    return parser;
}

// TODO: Replace this by calling the corresponding functions in interactive.cpp
static void execute_command(std::shared_ptr<FileShare::Peer> &peer, const std::string& cmd) {
    std::stringstream ss(cmd);
    std::string str;

    ss >> str;
    FileShare::Protocol::CommandCode command = FileShare::Protocol::str_to_command(str);

    ss >> str;
    // TODO: check nb arguments for each command
    switch (command) {
        case FileShare::Protocol::CommandCode::SEND_FILE: {
            peer->send_file(str);
            break;
        }
        case FileShare::Protocol::CommandCode::RECEIVE_FILE: {
            peer->receive_file(str);
            break;
        }
        case FileShare::Protocol::CommandCode::LIST_FILES: {
            peer->list_files(str);
            break;
        }
        // case FileShare::Protocol::CommandCode::PING: {
        // }
        default:
            std::cout << "Failed to run " << command << ". This type of command is not allowed." << std::endl;
    }
}

bool server_run = true;
void signal_handler([[maybe_unused]] int sig) {
    server_run = false;
}

// TODO: replace this testing loop with something better
static void run_server(FileShare::Server &server) {
    static const FileShare::Server::PeerAcceptCallback accept_cb = [](FileShare::Server &, std::shared_ptr<FileShare::PreAuthPeer> &) {
        // We accept everyone
        return true;
    };
    static const FileShare::Server::PeerRequestCallback request_cb = [](FileShare::Server &, std::shared_ptr<FileShare::Peer> &peer, FileShare::Protocol::Request &request){
        if (request.code == FileShare::Protocol::CommandCode::SEND_FILE || request.code == FileShare::Protocol::CommandCode::RECEIVE_FILE) {
            // We accept the file transfert
            peer->respond_to_request(request, FileShare::Protocol::StatusCode::STATUS_OK);
        } else { // (Ping)
            // Denying theses requests
            peer->respond_to_request(request, FileShare::Protocol::StatusCode::FORBIDDEN);
        }
    };

    std::signal(SIGINT, signal_handler);

    server_run = true;
    while (server_run) {
        server.process_events(accept_cb, request_cb);
    }
}

template <class Config> auto get_config(std::shared_ptr<argparse::ArgumentParser> &parser, std::string_view config_arg, std::string_view default_config_arg, Config default_config) -> Config {
    auto config_path = parser->present<std::string>(config_arg);

    if (parser->get<bool>(default_config_arg)) {
        return default_config;
    }
    if (config_path) {
        return Config::load(config_path.value());
    }
    return Config::load(); // Load default config path
}

static auto get_configs(std::shared_ptr<argparse::ArgumentParser> &parser, bool is_a_tty) -> std::pair<FileShare::ServerConfig, FileShare::Config> {
    FileShare::ServerConfig server_config;
    FileShare::Config peer_config;
    bool first_time_create_configs = false;

    try {
        server_config = get_config<FileShare::ServerConfig>(parser, SERVER_CONFIG_ARG, DEFAULT_SERVER_CONFIG_ARG, FileShare::Server::default_config());
    } catch (const FileShare::Errors::Config::NotFoundError &error) {
        if (parser->is_used(SERVER_CONFIG_ARG)) {
            throw error;
        }
        if (!is_a_tty) {
            std::cerr << "Server config not found and cannot ask to create it in non-interactive mode. Please create a config file or use " DEFAULT_SERVER_CONFIG_ARG " to create a new one with the default settings." << std::endl;
            throw error;
        }

        first_time_create_configs = interactive_user_question("Server config not found. Do you want to create a new one with default settings?");

        if (first_time_create_configs) {
            std::string device_name = interactive_user_input("Enter a distinguisable name for this device (will be visible by other peers)");

            if (device_name.empty()) { // User closed STDIN -> TODO Handle this better
                throw std::runtime_error("Device name cannot be empty");
            }
            server_config = FileShare::Server::default_config();
            server_config.set_device_name(device_name);
            std::cout << "Selected device name: " << std::quoted(device_name) << std::endl;
            server_config.save();
        } else {
            throw error;
        }
    }
    try {
        peer_config = get_config<FileShare::Config>(parser, CONFIG_ARG, DEFAULT_CONFIG_ARG, FileShare::Server::default_peer_config());
    } catch (const FileShare::Errors::Config::NotFoundError &error) {
        if (parser->is_used(CONFIG_ARG)) {
            throw error;
        }
        if (!first_time_create_configs) {
            std::cerr << "Default peer config not found. Check the permissions of the file or use " DEFAULT_CONFIG_ARG " to create a new one with default settings." << std::endl;
            throw error;
        }
        peer_config = FileShare::Server::default_peer_config();
        peer_config.save();
    }

    return {server_config, peer_config};
}

static void run(std::shared_ptr<argparse::ArgumentParser> &parser, bool is_a_tty) {
    auto cmds = parser->get<std::vector<std::string>>(EXECUTE_ARG);
    auto [config, peer_config] = get_configs(parser, is_a_tty);

    config.set_server_disabled(!parser->get<bool>(SERVER_ARG));
    FileShare::Server server(config, peer_config);

    if (parser->is_used(CONNECT_ARG)) {
        auto endpoint = parser->get<std::vector<std::string>>(CONNECT_ARG);
        int port = 12345;

        if (endpoint.size() == 2) {
            port = std::stoi(endpoint[1]);
        }
        std::shared_ptr<FileShare::Peer> peer = server.connect(CppSockets::EndpointV4(endpoint[0].c_str(), port));

        for (auto &cmd : cmds) {
            try {
                execute_command(peer, cmd);
            } catch (const std::exception &err) {
                std::cerr << "Failed to execute command: '" << cmd << "': " << err.what() << std::endl;
            }
        }
    }
    if (parser->get<bool>(INTERACTIVE_ARG) || (is_a_tty && !parser->is_used(EXECUTE_ARG))) {
        interactive_mode(server);
    } else if (!server.disabled()) {
        std::cout << "Running Server: Listening on " << server.get_server_endpoint().to_string() << " (Press CTRL+C to stop)" << std::endl;
        run_server(server);
    }
}

auto main(int argc, char *argv[]) -> int {
    std::shared_ptr<argparse::ArgumentParser> parser = setup_args();
    bool is_a_tty = isatty(fileno(stdin));

    try {
        parser->parse_args(argc, argv);
        if (parser->is_used(EXECUTE_ARG) && !parser->is_used(CONNECT_ARG)) {
            throw std::runtime_error(
                EXECUTE_ARG " cannot be used without a peer to connect to. Use "
                CONNECT_ARG " to provide it."
            );
        }
    } catch (const std::runtime_error &err) {
        std::cerr << "Argument error: " << err.what() << std::endl;
        std::cerr << std::endl;
        std::cerr << "Try -h for more information" << std::endl;
        return 1;
    }

    run(parser, is_a_tty);
    return 0;
}
