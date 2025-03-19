#include "device.hpp"

#include <spdlog/spdlog.h>

#include <iostream>
#include <unordered_set>

#include "link.hpp"

namespace sim {

void RoutingModule::add_inlink(std::shared_ptr<ILink> link) {
    if (link == nullptr) {
        spdlog::warn("Link is nullptr");
        return;
    }
    m_inlinks.insert(link);
    m_next_inlink = m_inlinks.begin();
}

void RoutingModule::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                         std::shared_ptr<ILink> link) {
    if (link == nullptr) {
        spdlog::warn("Unexpected nullptr link");
        return;
    }

    if (dest == nullptr) {
        spdlog::warn("Unexpected nullptr destination");
        return;
    }

    auto link_dest = link->get_to();
    if (link_dest == nullptr) {
        spdlog::warn("Unexpected nullptr link destination");
        return;
    }

    // TODO: discuss storing weak_ptrs instead of shared
    m_neighbours.insert(link_dest);
    m_routing_table[dest] = link;
}

std::vector<std::shared_ptr<IRoutingDevice>> RoutingModule::get_neighbours()
    const {
    std::unordered_set<std::shared_ptr<IRoutingDevice>> outlinks;
    for (auto device_link : m_routing_table) {
        outlinks.emplace((device_link.second)->get_to());
    }

    std::vector<std::shared_ptr<IRoutingDevice>> neighbours{};
    neighbours.insert(neighbours.begin(), outlinks.begin(), outlinks.end());
    return neighbours;
}

std::shared_ptr<ILink> RoutingModule::get_link_to_device(
    std::shared_ptr<IRoutingDevice> device) const {
    if (device == nullptr) {
        spdlog::warn("Unexpected nullptr device");
        return nullptr;
    }
    auto iterator = m_routing_table.find(device);
    if (iterator == m_routing_table.end()) {
        return nullptr;
    }

    return (*iterator).second;
}

std::shared_ptr<ILink> RoutingModule::next_inlink() {
    if (m_inlinks.empty()) {
        spdlog::info("Inlinks storage is empty");
        return nullptr;
    }

    std::shared_ptr<ILink> link = *m_next_inlink;
    if (++m_next_inlink == m_inlinks.end()) {
        m_next_inlink = m_inlinks.begin();
    }
    return link;
}

}  // namespace sim
