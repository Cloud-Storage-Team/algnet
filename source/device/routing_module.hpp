#pragma once

#include "device.hpp"

namespace sim {

class RoutingModule : public IRoutingDevice {
public:
    ~RoutingModule() = default;

    virtual bool add_inlink(std::shared_ptr<ILink> link) final;
    virtual bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                      std::shared_ptr<ILink> link) final;
    virtual std::vector<std::shared_ptr<IRoutingDevice>> get_neighbours()
        const final;
    virtual std::shared_ptr<ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> device) const final;

    // returns next inlink and moves inlinks set iterator forward
    std::shared_ptr<ILink> next_inlink() final;

private:
    // Ordered set as we need to iterate over the ingress buffers
    std::set<std::shared_ptr<ILink>> m_inlinks;

    // A routing table: maps the final destination to a specific link
    std::unordered_map<std::shared_ptr<IRoutingDevice>, std::shared_ptr<ILink>>
        m_routing_table;

    std::set<std::shared_ptr<IRoutingDevice>> m_neighbours;

    // Iterator for the next ingress to process
    std::set<std::shared_ptr<ILink>>::iterator m_next_inlink;
};

}  // namespace sim
