#pragma once

#include <memory>
#include <set>
#include <unordered_map>

#include "types.hpp"
#include "utils/loop_iterator.hpp"

namespace sim {

class ILink;

enum DeviceType { SWITCH, SENDER, RECEIVER };

class IProcessingDevice {
public:
    virtual ~IProcessingDevice() = default;

    // One step of device work cycle;
    // e.g. see next inlink, take one packet from it,
    // and do smth with it (send further, send ask etc)
    virtual Time process() = 0;

    virtual DeviceType get_type() const = 0;
};

class IRoutingDevice {
public:
    virtual ~IRoutingDevice() = default;

    virtual bool add_inlink(std::shared_ptr<ILink> link) = 0;
    virtual bool add_outlink(std::shared_ptr<ILink> link) = 0;
    virtual bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                      std::shared_ptr<ILink> link) = 0;
    virtual std::shared_ptr<ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> device) const = 0;
    virtual std::shared_ptr<ILink> next_inlink() = 0;
    virtual std::set<std::shared_ptr<ILink>> get_outlinks() const = 0;
};

class RoutingModule : public IRoutingDevice {
public:
    ~RoutingModule() = default;

    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool add_outlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<ILink> link) final;
    // returns next inlink and moves inlinks set iterator forward
    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> dest) const final;
    std::set<std::shared_ptr<ILink>> get_outlinks() const final;

private:
    // Ordered set as we need to iterate over the ingress buffers
    std::set<std::shared_ptr<ILink>> m_inlinks;

    std::set<std::shared_ptr<ILink>> m_outlinks;

    // A routing table: maps the final destination to a specific link
    std::unordered_map<std::shared_ptr<IRoutingDevice>, std::shared_ptr<ILink>>
        m_routing_table;

    // Iterator for the next ingress to process
    LoopIterator<std::set<std::shared_ptr<ILink>>::iterator> m_next_inlink;
};

}  // namespace sim
