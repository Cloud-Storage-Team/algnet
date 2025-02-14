#pragma once

#include <set>
#include <unordered_map>

#include "flow.hpp"
#include "link.hpp"
#include "packet.hpp"

namespace sim {

enum DeviceType { SWITCH, SENDER, RECEIVER };

class Device {
public:
    Device(DeviceType a_type);
    virtual ~Device();
    virtual void process();

    void add_inlink(Link* link);
    void add_outlink(Link* link, Device* dest);

    // TODO: add update_routing_table method
    // TODO: add get_neighbors method

private:
    DeviceType m_type;

    // Ordered set as we need to iterate over the ingress buffers
    std::set<Link*> m_inlinks;

    // Iterator for the next ingress to process
    std::set<Link*>::iterator m_next_inlink;

    // A routing table: maps destination node to a specific link
    std::unordered_map<Device*, Link*> m_outlinks;
};

}  // namespace sim
