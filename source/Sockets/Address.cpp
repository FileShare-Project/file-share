/*
** Project FileShare, 2022
**
** Author Francois Michaut
**
** Started on  Sun Feb 13 22:03:32 2022 Francois Michaut
** Last update Sun Feb 13 22:15:39 2022 Francois Michaut
**
** Address.cpp : Implementation of generic Address classes & functions
*/

#include "Sockets/Address.hpp"

namespace FileShare::Network {
    std::string IEndpoint::makeString() const {
        return this->getAddr().toString() + ":" + std::to_string(this->getPort());
    }
}
