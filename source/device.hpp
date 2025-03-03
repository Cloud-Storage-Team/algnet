#pragma once

#include <set>
#include <unordered_map>

namespace sim {

class Link;

enum DeviceType { SWITCH, SENDER, RECEIVER };

class IProcessableDevice {
public:
    virtual ~IProcessableDevice() = default;

    // One step of device work cycle;
    // e.g. see next inlink, take one packet from it,
    // and do smth with it (send further, send ask etc)
    virtual void process() = 0;

    virtual DeviceType get_type() const = 0;
};

class IRoutingDevice {
public:
    virtual ~IRoutingDevice() = default;

    virtual void add_inlink(Link *link) = 0;
    virtual void update_routing_table(IRoutingDevice *dest, Link *link) = 0;
    virtual Link *next_inlink() = 0;
};

// Default relisation of IDevice
// No override for process() !!!
class RountingModule : public IRoutingDevice {
public:
    ~RountingModule() = default;

    void add_inlink(Link *link) final;
    void update_routing_table(IRoutingDevice *dest, Link *link) final;

    // returns next inlink and moves inlinks set iterator forward
    Link *next_inlink() final;

private:
    // Ordered set as we need to iterate over the ingress buffers
    std::set<Link *> m_inlinks;

    // A routing table: maps the final destination to a specific link
    std::unordered_map<IRoutingDevice *, Link *> m_routing_table;

    // Iterator for the next ingress to process
    std::set<Link *>::iterator m_next_inlink;
};

}  // namespace sim
