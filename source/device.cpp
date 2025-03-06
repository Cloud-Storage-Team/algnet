#include "device.hpp"

#include <unordered_set>

#include "link.hpp"

namespace sim {

Device::Device(DeviceType a_type) : m_type(a_type) {
    m_inlinks = std::set<Link*>(),
    m_routing_table = std::unordered_map<Device*, Link*>();
    m_next_inlink = m_inlinks.begin();
}

void Device::add_inlink(Link* link) {
    m_inlinks.insert(link);
    m_next_inlink = m_inlinks.begin();
}

void Device::update_routing_table(Device* dest, Link* link) {
    m_routing_table[dest] = link;
}

std::vector<Device*> Device::get_neighbors() const {
    std::unordered_set<Device*> outlinks;
    for (auto device_link : m_routing_table) {
        outlinks.emplace((device_link.second)->get_dest());
    }

    std::vector<Device*> neighbours{};
    neighbours.insert(neighbours.begin(), outlinks.begin(), outlinks.end());
    return neighbours;
}

Link* Device::get_link_to_device(Device* device) const {
    auto iterator = m_routing_table.find(device);
    if (iterator == m_routing_table.end()) {
        return nullptr;
    }

    return (*iterator).second;
}

}  // namespace sim
