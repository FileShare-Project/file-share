/*
** Project FileShare, 2022
**
** Author Francois Michaut
**
** Started on  Sat Jan 15 21:08:44 2022 Francois Michaut
** Last update Thu Aug 25 22:39:33 2022 Francois Michaut
**
** LanDiscovery.hpp : Class to discover other pairs on LAN with multicast.
*/

#include <unordered_set>

namespace FileShare::Network {
    class IPair {
        public:

    };

    template<class Pair>
    class LanDiscovery {
        static_assert(
            std::is_base_of<IPair, Pair>::value,
            "LAN_Discovery pairs must derive from IPair"
        );

        public:
            static constexpr char const *MULTICAST_GROUP = "234.170.170.170";
            static constexpr int MULTICAST_PORT = 4242;
            static constexpr char const *HEADER_MAGIC = "3@FSP";

            LanDiscovery();
            LanDiscovery(char const *multicast_group, int port, char const *header_magic);
            ~LanDiscovery();

            // TODO maybe change
            LanDiscovery(LanDiscovery &other) = delete;
            LanDiscovery(LanDiscovery &&other) = delete;
            LanDiscovery &operator=(LanDiscovery &other) = delete;
            LanDiscovery &operator=(LanDiscovery &&other) = delete;

            const std::unordered_set<Pair> &get_pairs() const;
        private:
            std::unordered_set<Pair> pairs;
    };
}
