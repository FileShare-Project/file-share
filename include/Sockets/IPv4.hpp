/*
** Project FileShare, 2022
**
** Author Francois Michaut
**
** Started on  Sun Feb 13 17:05:02 2022 Francois Michaut
** Last update Sun Feb 13 22:00:28 2022 Francois Michaut
**
** IPv4.hpp : Class used to represent and manipulate IPv4 addresses
*/

#pragma once

#include "Sockets/Address.hpp"

namespace FileShare::Network {
    class IPv4 : public IAddress {
        public:
            IPv4(std::uint32_t addr);
            IPv4(const char *addr);

            [[nodiscard]]
            std::uint32_t getAddress() const override;

            [[nodiscard]]
            int getFamily() const override;

            [[nodiscard]]
            const std::string &toString() const override;

        private:
            std::uint32_t addr;
            std::string str;
    };
}
