/*
** Project FileShare, 2023
**
** Author Francois Michaut
**
** Started on  Sat Nov 11 11:06:03 2023 Francois Michaut
** Last update Thu Nov 23 23:19:41 2023 Francois Michaut
**
** main.cpp : Main entry point of FileShare CLI
*/

#include "FileShareVersion.hpp"
#include "FileShare/Server.hpp"

#include <argparse/argparse.hpp>

#include <csignal>
#include <memory>
#include <iostream>

#define DESCRIPTION ""

#define SERVER_ARG "--server"
#define CONNECT_ARG "--connect"
#define INTERACTIVE_ARG "--interactive"
#define EXECUTE_ARG "--execute"

#include <filesystem>

static std::shared_ptr<argparse::ArgumentParser> setup_args()
{
    std::shared_ptr<argparse::ArgumentParser> parser = std::make_shared<argparse::ArgumentParser>("file-share-cli", FILE_SHARE_VERSION);

    parser->add_description(DESCRIPTION);

    parser->add_argument("-s", SERVER_ARG)
        .flag()
        .help("Enable server, allowing other clients to connect. The program will keep running until killed");

    parser->add_argument("-c", CONNECT_ARG)
        .nargs(1, 2)
        .help("Connect to an external server. Format '-c ip [port]'. Required to execute commands.");

    parser->add_argument("-e", EXECUTE_ARG)
        .append()
        .help("Add a command to be executed. Repeat the flag to add multiple commands");

    parser->add_argument("-i", INTERACTIVE_ARG)
        .flag()
        .help("Enable interactive mode. You will be able to change settings and execute commands at will");
    return parser;
}

static void execute_command(FileShare::Server &server, std::shared_ptr<FileShare::Client> &client, std::string cmd)
{
    std::stringstream ss(cmd);
    std::string str;

    ss >> str;
    FileShare::Protocol::CommandCode command = FileShare::Protocol::str_to_command(str);

    ss >> str;
    // TODO: check nb arguments for each command
    switch (command) {
        case FileShare::Protocol::CommandCode::SEND_FILE: {
            client->send_file(str);
            break;
        }
        case FileShare::Protocol::CommandCode::RECEIVE_FILE: {
            client->receive_file(str);
            break;
        }
        case FileShare::Protocol::CommandCode::LIST_FILES: {
            client->list_files(str);
            break;
        }
        default:
            std::cout << "Failed to run " << command << ". This type of command is not allowed." << std::endl;
    }
}

static bool server_run = true;
static void signal_handler(int sig) {
    server_run = false;
}

// TODO: replace this testing loop with something better
static void run_server(FileShare::Server &server) {
    FileShare::Server::Event e;

    std::signal(SIGINT, signal_handler);

    server_run = true;
    while (server_run) {
        if (server.pull_event(e)) {
            if (e.request().has_value()) {
                auto &request = e.request().value();
                if (request.code == FileShare::Protocol::CommandCode::SEND_FILE || request.code == FileShare::Protocol::CommandCode::RECEIVE_FILE) {
                    // We accept the file transfert
                    e.client()->respond_to_request(request, FileShare::Protocol::StatusCode::STATUS_OK);
                } else {
                    // Denying theses requests
                    e.client()->respond_to_request(request, FileShare::Protocol::StatusCode::FORBIDDEN);
                }
            } else {
                // We accept everyone
                server.accept_client(std::move(e.client()));
            }
        }
    }
}

static void run(std::shared_ptr<argparse::ArgumentParser> &parser)
{
    FileShare::Config config = FileShare::Server::default_config();
    auto cmds = parser->get<std::vector<std::string>>(EXECUTE_ARG);

    config.set_server_disabled(!parser->get<bool>(SERVER_ARG));
    FileShare::Server server(config);

    if (parser->is_used(CONNECT_ARG)) {
        std::vector<std::string> endpoint = parser->get<std::vector<std::string>>(CONNECT_ARG);
        int port = 12345;

        if (endpoint.size() == 2) {
            port = std::stoi(endpoint[1]);
        }
        std::shared_ptr<FileShare::Client> client = server.connect(CppSockets::EndpointV4(endpoint[0].c_str(), port));

        for (auto &cmd : cmds) {
            execute_command(server, client, cmd);
        }
    }
    if (parser->get<bool>(INTERACTIVE_ARG)) {
        // TODO: interactive mode
    } else if (!server.disabled()) {
        std::cout << "Running Server: Listening on " << server.get_server_endpoint().toString() << " (Press CTRL+C to stop)" << std::endl;
        run_server(server);
    }
}

int main(int argc, char *argv[])
{
    std::shared_ptr<argparse::ArgumentParser> parser = setup_args();

    try {
        parser->parse_args(argc, argv);
        if (parser->is_used(EXECUTE_ARG) && !parser->is_used(CONNECT_ARG)) {
            throw std::runtime_error(EXECUTE_ARG " cannot be used without a peer to connect to. Use " CONNECT_ARG " to provide it.");
        }
    } catch (const std::runtime_error &err) {
        std::cerr << "Argument error: " << err.what() << std::endl;
        std::cerr << std::endl;
        std::cerr << "Try -h for more information" << std::endl;
        std::exit(1);
    }

    run(parser);
    return 0;
}
