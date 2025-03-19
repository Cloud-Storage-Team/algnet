#pragma once

#include <memory>

#include "device.hpp"

namespace sim {

class ISwitch : public IRoutingDevice, IProcessingDevice {
public:
    virtual ~ISwitch() = default;
};

class Switch : public ISwitch {
public:
    Switch() = default;
    ~Switch() = default;

    void add_inlink(std::shared_ptr<ILink> link) final;
    void update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<ILink> link) final;

    DeviceType get_type() const final;
    // Process a packet by moving it from ingress to egress
    // and schedule next process event after a delay.
    // Packets are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    void process() final;

private:
    std::unique_ptr<IRoutingDevice> m_router;
};

}  // namespace sim
