#pragma once
#include <optional>
#include <utility>

#include "network/connection/flow/packet.hpp"
#include "topology/link/i_link.hpp"

namespace sim {

// Stores links from a specific device that belong to equal-cost paths
// to a specific destination device.
class EcmpNextHops {
public:
    EcmpNextHops();

    void add_link(std::shared_ptr<ILink> link, std::size_t routes_count);

    std::shared_ptr<ILink> operator[](std::size_t hash) const;

    std::size_t get_max_hash() const;

private:
    struct Cache {
        // gcd of all weights in m_next_hops
        std::size_t weights_gcd;

        // If links_table[hash] is a link such round-robin
        // hash (aka sum of weights that are lying in m_next_hops before it
        // divided by weights_gcd) correspond to given one
        std::vector<std::shared_ptr<ILink> > links_table;
    };

    // recreates m_cache and returns reference to its value
    const Cache& recalc_cache() const;

    std::vector<std::pair<std::shared_ptr<ILink>, std::size_t> > m_next_hops;
    std::size_t m_total_weight;

    mutable std::optional<Cache> m_cache;
};

}  // namespace sim