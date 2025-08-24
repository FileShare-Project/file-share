/*
** Project FileShare, 2023
**
** Author Francois Michaut
**
** Started on  Sat Nov 11 11:06:03 2023 Francois Michaut
** Last update Fri Aug 22 17:19:40 2025 Francois Michaut
**
** main.cpp : Main entry point of FileShare CLI
*/

#include "FileShareVersion.hpp"

#include <FileShare/Config/ServerConfig.hpp>
#include <FileShare/Peer/PreAuthPeer.hpp>
#include <FileShare/Server.hpp>

#include <argparse/argparse.hpp>

#include <csignal>
#include <iostream>
#include <memory>

#define DESCRIPTION "" // TODO

#define SERVER_ARG "--server"
#define CONNECT_ARG "--connect"
#define INTERACTIVE_ARG "--interactive"
#define EXECUTE_ARG "--execute"
#define CONFIG_ARG "--config"
#define SERVER_CONFIG_ARG "--server-config"

void interactive_mode(FileShare::Server &server);

static auto setup_args() -> std::shared_ptr<argparse::ArgumentParser> {
    auto parser = std::make_shared<argparse::ArgumentParser>("file-share-cli", FILE_SHARE_VERSION);

    parser->add_description(DESCRIPTION);

    parser->add_argument("-s", SERVER_ARG)
        .flag()
        .help("Enable server, allowing other peers to connect. The program will keep running until killed.");

    parser->add_argument(SERVER_CONFIG_ARG)
        .default_value("")
        .help("Speficy the path to the server config file.");

    parser->add_argument(CONFIG_ARG)
        .default_value("")
        .help("Speficy the path to the peer config file.");

    parser->add_argument("-c", CONNECT_ARG)
        .nargs(1, 2)
        .help("Connect to an external server. Format '-c ip [port]'. Required to execute commands.");

    parser->add_argument("-e", EXECUTE_ARG)
        .append()
        .help("Add a command to be executed. Repeat the flag to add multiple commands.");

    parser->add_argument("-i", INTERACTIVE_ARG)
        .flag()
        .help("Enable interactive mode. You will be able to change settings and execute commands from a CLI.");

    // parser->add_epilog(); // TODO ?

    return parser;
}

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

static void run(std::shared_ptr<argparse::ArgumentParser> &parser) {
    auto server_config_path = parser->get<std::string>(SERVER_CONFIG_ARG);
    auto config_path = parser->get<std::string>(CONFIG_ARG);
    auto cmds = parser->get<std::vector<std::string>>(EXECUTE_ARG);

    FileShare::ServerConfig config = FileShare::ServerConfig::load(server_config_path);
    FileShare::Config peer_config = FileShare::Config::load(config_path);

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
    if (parser->get<bool>(INTERACTIVE_ARG)) {
        interactive_mode(server);
    } else if (!server.disabled()) {
        std::cout << "Running Server: Listening on " << server.get_server_endpoint().to_string() << " (Press CTRL+C to stop)" << std::endl;
        run_server(server);
    }
}

auto main(int argc, char *argv[]) -> int {
    std::shared_ptr<argparse::ArgumentParser> parser = setup_args();

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

    run(parser);
    return 0;
}
