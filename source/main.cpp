/*
** Project FileShare, 2022
**
** Author Francois Michaut
**
** Started on  Sat Jan 15 01:26:14 2022 Francois Michaut
** Last update Sat Jul 22 23:21:45 2023 Francois Michaut
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
        if (s.pull_event(e)) {
            if (e.request().has_value()) {
                std::cout << "REQUEST !" << std::endl;
                std::cout << e.request().value().request->to_str() << std::endl;
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
    sleep(5);
    client->list_files("/some/path", 2);
    client->receive_file("/etc/passwd");
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
