/*
** Project FileShare, 2022
**
** Author Francois Michaut
**
** Started on  Sat Jan 15 01:17:42 2022 Francois Michaut
** Last update Tue Feb 15 21:30:44 2022 Francois Michaut
**
** Socket.hpp : Portable C++ socket class
*/

#pragma once

#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
using RawSocketType=SOCKET;
using socklen_t=int;
#else
#include <sys/socket.h>
using RawSocketType=int;
#endif

#include <memory>

#include "Sockets/Address.hpp"

namespace FileShare::Network {
    class Socket {
        public:
            Socket(int domain, int type, int protocol);
            ~Socket();

            Socket(Socket &other) = delete;
            Socket(Socket &&other) noexcept;
            Socket &operator=(const Socket &other) = delete;
            Socket &operator=(Socket &&other) noexcept;

            std::string read(std::size_t len = -1);
            int read_buff(char *buff, std::size_t size);
            int write(const std::string &buff);
            int write(const char *buff, std::size_t len);

            int getsockopt(int level, int optname, void *optval, socklen_t *optlen);
            int setsockopt(int level, int optname, const void *optval, socklen_t optlen);

            int connect(const IEndpoint &endpoint);
            int connect(const std::string &addr, int port);

            int bind(int addr, int port); // TODO remove ?
            int bind(const std::string &addr, int port);
            int listen(int backlog);
            std::shared_ptr<Socket> accept(void *addr_out);

            void setNoBlock(bool val);

            [[nodiscard]]
            int getFd() const;
            [[nodiscard]]
            int getType() const;
            [[nodiscard]]
            int getDomain() const;
            [[nodiscard]]
            int getProtocol() const;

            [[nodiscard]]
            bool isConnected() const;
        private:
            Socket(int domain, int type, int protocol, int sockfd);

            static void init();

            static int getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen);
            static int get_errno();
            static char *strerror(int err);
            static char *strerror();

            int domain;
            int type;
            int protocol;
            RawSocketType sockfd;
            bool is_connected = false;
    };
}
