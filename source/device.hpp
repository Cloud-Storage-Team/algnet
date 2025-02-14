#pragma once

#include <set>
#include <unordered_map>

namespace sim {

class Link;

enum DeviceType { SWITCH, SENDER, RECEIVER };

class Device {
public:
    Device(DeviceType a_type);
    virtual ~Device();
    virtual void process();

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
