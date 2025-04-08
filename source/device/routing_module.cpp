#include "routing_module.hpp"
#include "logger/logger.hpp"

#include <iostream>
#include <unordered_set>

#include "link.hpp"

namespace sim {

bool RoutingModule::add_inlink(std::shared_ptr<ILink> link) {
    if (m_inlinks.contains(link)) {
        LOG_WARN("Unexpected already added inlink");
        return false;
    }
    m_inlinks.insert(link);
    m_next_inlink = m_inlinks.begin();
    return true;
}

bool RoutingModule::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                         std::shared_ptr<ILink> link) {
    if (link == nullptr) {
        LOG_WARN("Unexpected nullptr link");
        return false;
    }
    auto link_dest = link->get_to();

    // TODO: discuss storing weak_ptrs instead of shared
    m_neighbours.insert(link_dest);
    m_routing_table[dest] = link;
    return true;
}

std::vector<std::shared_ptr<IRoutingDevice>> RoutingModule::get_neighbours()
    const {
    std::vector<std::shared_ptr<IRoutingDevice>> neighbours{};
    neighbours.insert(neighbours.begin(), m_neighbours.begin(),
                      m_neighbours.end());
    return neighbours;
}

std::shared_ptr<ILink> RoutingModule::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> device) const {
    auto iterator = m_routing_table.find(device);
    if (iterator == m_routing_table.end()) {
        return nullptr;
    }

    return (*iterator).second;
}

std::shared_ptr<ILink> RoutingModule::next_inlink() {
    if (m_inlinks.empty()) {
        LOG_INFO("Inlinks storage is empty");
        return nullptr;
    }

    std::shared_ptr<ILink> link = *m_next_inlink;
    if (++m_next_inlink == m_inlinks.end()) {
        m_next_inlink = m_inlinks.begin();
    }
    return link;
}

}  // namespace sim
