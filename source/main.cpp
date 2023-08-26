/*
** Project FileShare, 2022
**
** Author Francois Michaut
**
** Started on  Sat Jan 15 01:26:14 2022 Francois Michaut
** Last update Thu Oct 26 21:00:22 2023 Francois Michaut
**
** main.cpp : Main entry point
*/

#include <CppSockets/Socket.hpp>
#include <CppSockets/Version.hpp>

#include <FileShare/Server.hpp>
#include <FileShare/Utils/FileHash.hpp>
#include <FileShare/Protocol/RequestData.hpp>

#include <iostream>

void run_server() {
    FileShare::Server s;
    FileShare::Server::Event e;

    while (true) {
        std::cout << "Nb Clients: " << s.get_clients().size() << std::endl;
        if (s.pull_event(e)) {
            if (e.request().has_value()) {
                auto &request = e.request().value();
                std::cout << "REQUEST: " << (int)request.message_id << " " << request.request->debug_str() << std::endl;
                if (request.code == FileShare::Protocol::CommandCode::SEND_FILE || request.code == FileShare::Protocol::CommandCode::RECEIVE_FILE) {
                    // We accept the file transfert
                    e.client()->respond_to_request(request, FileShare::Protocol::StatusCode::STATUS_OK);
                } else {
                    // Denying theses requests
                    e.client()->respond_to_request(request, FileShare::Protocol::StatusCode::FORBIDDEN);
                }
            } else {
                // We accept everyone
                std::cout << "New Client !" << std::endl;
                s.accept_client(std::move(e.client()));
            }
        }
    }
}

void run_client() {
    FileShare::Config config = FileShare::Server::default_config();
    config.set_server_disabled(true);

    FileShare::Server s(config);
    std::cout << "Connecting..." << std::endl;
    std::shared_ptr<FileShare::Client> client = s.connect(CppSockets::EndpointV4("127.0.0.1", 12345));

    std::cout << "Pull..." << std::endl;
    // client->pull_requests(); // TODO: make it so that it does not block
    std::cout << "Pull done !" << std::endl;

    client->list_files();
    client->list_files("/some/path", 2);
    client->send_file("/tmp/to_send");
    client->receive_file("/etc/passwd"); // Should get rejected by the server since it is a sensitive file

    sleep(2); // TODO: without this, server does not receive the last request... Investigate why and if it is fixable
}

int main(int ac, char **av)
{
    if (ac == 1) {
        std::cout << "RUNNING SERVER" << std::endl;
        run_server();
    } else {
        std::cout << "RUNNING CLIENT" << std::endl;
        run_client();
    }
}
