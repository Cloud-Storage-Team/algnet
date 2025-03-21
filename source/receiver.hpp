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

    void add_inlink(std::shared_ptr<ILink> link) final;
    void update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<ILink> link) final;

    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> dest) const final;
    std::vector<std::shared_ptr<ILink>> get_outlinks();

    DeviceType get_type() const final;
    // Process a packet by removing it from the ingress buffer
    // Send an ACK to the egress buffer
    // and schedule next receive event after a delay.
    // Upon receiving send an ACK to the sender.
    // Packets are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    void process() final;

private:
    std::unique_ptr<IRoutingDevice> m_router;
};

}  // namespace sim
