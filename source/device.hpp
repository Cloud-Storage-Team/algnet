#pragma once

#include <set>
#include <unordered_map>

namespace sim {

class Link;

class IDevice {
public:
    IDevice();
    virtual ~IDevice();

    // See ingress buffer of next inlink,
    // take one packet from it and do smth with it:
    // send further, send ASK etc.
    // Depends on device type
    virtual void process() = 0;
    virtual void add_inlink(Link *link) = 0;
    virtual void update_routing_table(IDevice *dest, Link *link) = 0;
};

enum DeviceType { SWITCH, SENDER, RECEIVER };

class Device : public IDevice {
public:
    Device(DeviceType a_type);
    virtual ~Device();
    virtual void process() = 0;
    void add_inlink(Link *link) final;
    void update_routing_table(IDevice *dest, Link *link) final;

protected:
    DeviceType m_type;

    // Ordered set as we need to iterate over the ingress buffers
    std::set<Link *> m_inlinks;

    // Iterator for the next ingress to process
    std::set<Link *>::iterator m_next_inlink;

    // A routing table: maps the final destination to a specific link
    std::unordered_map<IDevice *, Link *> m_routing_table;
};

}  // namespace sim
