#include "device.hpp"

#include <iostream>
#include <unordered_set>

#include "link.hpp"

namespace sim {

void RoutingModule::add_inlink(std::shared_ptr<Link> link) {
    if (link == nullptr) {
         //TODO: add warning to log
           return;
    }
    m_inlinks.insert(link);
    m_next_inlink = m_inlinks.begin();
}

void RoutingModule::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                         std::shared_ptr<Link> link) {
    m_routing_table[dest] = link;
}

std::vector<std::shared_ptr<IRoutingDevice>> RoutingModule::get_neighbours()
    const {
    std::unordered_set<std::shared_ptr<IRoutingDevice>> outlinks;
    for (auto device_link : m_routing_table) {
        outlinks.emplace((device_link.second)->get_dest());
    }

    std::vector<std::shared_ptr<IRoutingDevice>> neighbours{};
    neighbours.insert(neighbours.begin(), outlinks.begin(), outlinks.end());
    return neighbours;
}

std::shared_ptr<Link> RoutingModule::get_link_to_device(
    std::shared_ptr<IRoutingDevice> device) const {
    auto iterator = m_routing_table.find(device);
    if (iterator == m_routing_table.end()) {
        return nullptr;
    }

    return (*iterator).second;
}

std::shared_ptr<Link> RoutingModule::next_inlink() {
    if (m_inlinks.empty()) {
        return nullptr;
    }

    std::shared_ptr<Link> link = *m_next_inlink;
    if (++m_next_inlink == m_inlinks.end()) {
        m_next_inlink = m_inlinks.begin();
    }
    return link;
}

}  // namespace sim
