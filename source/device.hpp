#pragma once

#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

namespace sim {

class ILink;

enum DeviceType { SWITCH, SENDER, RECEIVER };

class IProcessingDevice {
public:
    virtual ~IProcessingDevice() = default;

    // One step of device work cycle;
    // e.g. see next inlink, take one packet from it,
    // and do smth with it (send further, send ask etc)
    virtual uint32_t process() = 0;

    virtual DeviceType get_type() const = 0;
};

class IRoutingDevice {
public:
    virtual ~IRoutingDevice() = default;

    virtual void add_inlink(std::shared_ptr<ILink> link) = 0;
    virtual void update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                      std::shared_ptr<ILink> link) = 0;
    virtual std::vector<std::shared_ptr<IRoutingDevice>> get_neighbours()
        const = 0;
    virtual std::shared_ptr<ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> device) const = 0;
    virtual std::shared_ptr<ILink> next_inlink() = 0;
};

class RoutingModule : public IRoutingDevice {
public:
    ~RoutingModule() = default;

    virtual void add_inlink(std::shared_ptr<ILink> link) final;
    virtual void update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                      std::shared_ptr<ILink> link) final;
    virtual std::vector<std::shared_ptr<IRoutingDevice>> get_neighbours()
        const final;
    virtual std::shared_ptr<ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> device) const final;

    // returns next inlink and moves inlinks set iterator forward
    std::shared_ptr<ILink> next_inlink() final;

private:
    // Ordered set as we need to iterate over the ingress buffers
    std::set<std::shared_ptr<ILink>> m_inlinks;

    // A routing table: maps the final destination to a specific link
    std::unordered_map<std::shared_ptr<IRoutingDevice>, std::shared_ptr<ILink>>
        m_routing_table;

    std::set<std::shared_ptr<IRoutingDevice>> m_neighbours;

    // Iterator for the next ingress to process
    std::set<std::shared_ptr<ILink>>::iterator m_next_inlink;
};

}  // namespace sim
