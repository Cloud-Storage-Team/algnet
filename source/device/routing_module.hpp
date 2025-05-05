#pragma once

#include <map>

#include "device.hpp"
#include "types.hpp"
#include "utils/loop_iterator.hpp"

namespace sim {

class RoutingModule : public IRoutingDevice {
public:
    ~RoutingModule() = default;

    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool add_outlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<ILink> link) final;
    // returns next inlink and moves inlinks set iterator forward
    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> dest) const final;
    std::set<std::shared_ptr<ILink>> get_outlinks() final;

    void correctify_inlinks();
    void correctify_outlinks();

private:
    // Ordered set as we need to iterate over the ingress buffers
    std::set<std::weak_ptr<ILink>, std::owner_less<std::weak_ptr<ILink>>>
        m_inlinks;

    std::set<std::weak_ptr<ILink>, std::owner_less<std::weak_ptr<ILink>>>
        m_outlinks;

    // A routing table: maps the final destination to a specific link
    std::map<std::weak_ptr<IRoutingDevice>, std::weak_ptr<ILink>,
             std::owner_less<std::weak_ptr<IRoutingDevice>>>
        m_routing_table;

    // Iterator for the next ingress to process
    LoopIterator<std::set<std::weak_ptr<ILink>,
                          std::owner_less<std::weak_ptr<ILink>>>::iterator>
        m_next_inlink;
};

}  // namespace sim
