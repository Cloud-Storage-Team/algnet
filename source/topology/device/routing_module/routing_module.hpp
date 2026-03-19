#pragma once

#include <unordered_map>

#include "../hashers/i_hasher.hpp"
#include "ecmp_next_hops.hpp"
#include "topology/device/interfaces/i_routing_device.hpp"
#include "utils/loop_iterator.hpp"

namespace sim {

class RoutingModule : public virtual IRoutingDevice {
public:
    RoutingModule(Id a_id = "",
                  std::unique_ptr<IPacketHasher> a_hasher = nullptr);
    virtual ~RoutingModule() = default;

    const Id& get_id() const final;
    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool add_outlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(Id dest_id, std::shared_ptr<ILink> link,
                              size_t paths_count = 1) final;
    // returns next inlink and moves inlinks set iterator forward
    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(
        const Packet& packet) const final;
    std::set<std::shared_ptr<ILink>> get_outlinks() final;

private:
    Id m_id;
    std::unique_ptr<IPacketHasher> m_hasher;

    using LinksStorage = std::set<std::shared_ptr<ILink>>;

    // Ordered set as we need to iterate over the ingress buffers
    LinksStorage m_inlinks;

    LinksStorage m_outlinks;

    // A routing table: maps the final destination to a specific link
    std::unordered_map<IdWithHash, EcmpNextHops> m_routing_table;

    // Iterator for the next ingress to process
    LoopIterator<LinksStorage::iterator> m_next_inlink;
};

}  // namespace sim
