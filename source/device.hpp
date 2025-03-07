#pragma once

#include <memory>
#include <vector>
#include <set>
#include <unordered_map>

namespace sim {

class Link;

enum DeviceType { SWITCH, SENDER, RECEIVER };

class IProcessingDevice {
public:
    virtual ~IProcessingDevice() = default;

    // One step of device work cycle;
    // e.g. see next inlink, take one packet from it,
    // and do smth with it (send further, send ask etc)
    virtual void process() = 0;

    virtual DeviceType get_type() const = 0;
};

class IRoutingDevice {
public:
    virtual ~IRoutingDevice() = default;

    virtual void add_inlink(std::shared_ptr<Link> link) = 0;
    virtual void update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                      std::shared_ptr<Link> link) = 0;
    virtual std::shared_ptr<Link> next_inlink() = 0;
    virtual std::shared_ptr<Link> get_link_to_device(IRoutingDevice* a_device) = 0;
    virtual std::vector<IRoutingDevice*> get_neighbors() = 0;    
};

class RoutingModule : public IRoutingDevice {
public:
    ~RoutingModule() = default;

    virtual void add_inlink(std::shared_ptr<Link> link) final;
    virtual void update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                      std::shared_ptr<Link> link) final;

    // returns next inlink and moves inlinks set iterator forward
    std::shared_ptr<Link> next_inlink() final;

private:
    // Ordered set as we need to iterate over the ingress buffers
    std::set<std::shared_ptr<Link> > m_inlinks;

    // A routing table: maps the final destination to a specific link
    std::unordered_map<std::shared_ptr<IRoutingDevice>, std::shared_ptr<Link> >
        m_routing_table;

    // Iterator for the next ingress to process
    std::set<std::shared_ptr<Link> >::iterator m_next_inlink;
};

}  // namespace sim
