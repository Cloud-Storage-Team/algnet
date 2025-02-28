#pragma once

#include <set>
#include <unordered_map>
#include <vector>

namespace sim {

class Link;

enum DeviceType { SWITCH, SENDER, RECEIVER };

class Device {
public:
    Device(DeviceType a_type);
    virtual ~Device() = default;
    virtual void process() = 0;
    void add_inlink(Link *link);
    void update_routing_table(Device *dest, Link *link);
    std::vector<Device *> get_neighbors() const;
    Link *get_link_to_device(Device *device) const;

protected:
    DeviceType m_type;

    // TODO: update to shared and weak pointers

    // Ordered set as we need to iterate over the ingress buffers
    std::set<Link *> m_inlinks;

    // Iterator for the next ingress to process
    std::set<Link *>::iterator m_next_inlink;

    // TODO: think about a problem of having 2 pointers storing the same object
    // and being unable to find link when passing pointer different from one
    // which was used for updating routing table

    // A routing table: maps the final destination to a specific link
    std::unordered_map<Device *, Link *> m_routing_table;
};

}  // namespace sim
