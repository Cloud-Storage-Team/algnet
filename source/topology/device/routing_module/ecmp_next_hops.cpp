#include "ecmp_next_hops.hpp"

#include <numeric>

namespace sim {

EcmpNextHops::EcmpNextHops() : m_total_weight(0), m_cache(std::nullopt) {}

void EcmpNextHops::add_link(std::shared_ptr<ILink> link,
                            std::size_t routes_count) {
    m_cache.reset();
    m_total_weight += routes_count;
    for (auto& [old_link, old_routes_count] : m_next_hops) {
        bool same_link =
            !old_link.owner_before(link) && !link.owner_before(old_link);

        if (same_link) {
            old_routes_count += routes_count;
        }
    }
}

std::shared_ptr<ILink> EcmpNextHops::operator[](std::size_t hash) const {
    const Cache& cache =
        (m_cache.has_value() ? m_cache.value() : recalc_cache());

    std::size_t max_hash = get_max_hash();

    if (hash >= max_hash) {
        throw std::runtime_error(fmt::format(
            "Given hash {} is grateater or equal to maximal one ({})", hash,
            max_hash));
    }
    
    return cache.links_table[hash];
}

std::size_t EcmpNextHops::get_max_hash() const {
    const Cache& cache =
        (m_cache.has_value() ? m_cache.value() : recalc_cache());

    return m_total_weight / cache.weights_gcd;
}

const EcmpNextHops::Cache& EcmpNextHops::recalc_cache() const {
    std::size_t gcd = 0;
    for (const auto& [_, weight] : m_next_hops) {
        gcd = std::gcd(gcd, weight);
    }

    std::vector<std::shared_ptr<ILink> > cache;
    cache.reserve(m_total_weight / gcd);

    for (const auto& [link, weight] : m_next_hops) {
        std::size_t copies_count = weight / gcd;
        for (std::size_t i = 0; i < copies_count; i++) {
            cache.emplace_back(link);
        }
    }

    return m_cache.emplace(Cache{gcd, std::move(cache)});
}

}  // namespace sim