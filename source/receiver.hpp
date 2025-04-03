#pragma once
#include <memory>

#include "device.hpp"

namespace sim {

class IReceiver : public IRoutingDevice, IProcessingDevice {
public:
    virtual ~IReceiver() = default;
};

class Receiver : public IReceiver {
public:
    Receiver() = default;
    ~Receiver() = default;

    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<ILink> link) final;

    DeviceType get_type() const final;
    // Process a packet by removing it from the ingress buffer
    // Send an ACK to the egress buffer
    // and schedule next receive event after a delay.
    // Upon receiving send an ACK to the sender.
    // Packets are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    std::uint32_t process() final;

private:
    std::unique_ptr<IRoutingDevice> m_router;
};

}  // namespace sim
