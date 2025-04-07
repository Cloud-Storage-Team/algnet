#pragma once
#include <queue>

#include "device.hpp"

namespace sim {

struct Packet;

class ISender : public IRoutingDevice, IProcessingDevice {
public:
    virtual ~ISender() = default;
    virtual void enqueue_packet(Packet packet) = 0;
};

class Sender : public ISender {
public:
    Sender() = default;
    ~Sender() = default;

    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<ILink> link) final;

    DeviceType get_type() const final;
    // Process an ACK by removing it from the ingress buffer,
    // update the flow state,
    // and schedule next ACK processing event after a delay.
    // Send the next packet: dequeue from the flow buffer
    // and move it to the egress port (link)
    // ACKs are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    Time process() final;

    void enqueue_packet(Packet packet) final;

private:
    std::queue<Packet> m_flow_buffer;
};

}  // namespace sim
