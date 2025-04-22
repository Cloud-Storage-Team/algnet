#pragma once

#include <memory>

#include "routing_module.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

class ISwitch : public IRoutingDevice,
                public IProcessingDevice,
                public Identifiable {
public:
    virtual ~ISwitch() = default;
};

class Switch : public ISwitch, public std::enable_shared_from_this<Switch> {
public:
    Switch();
    ~Switch() = default;

    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool add_outlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<ILink> link) final;
    bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::unordered_map<std::shared_ptr<ILink>, int> paths) final;
    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> dest) const final;
    std::set<std::shared_ptr<ILink>> get_outlinks() const final;

    DeviceType get_type() const final;
    // Process a packet by moving it from ingress to egress
    // and schedule next process event after a delay.
    // Packets are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    Time process() final;
    
    Id get_id() const final;

private:
    std::unique_ptr<RoutingModule> m_router;
    Id m_id;
};

}  // namespace sim
