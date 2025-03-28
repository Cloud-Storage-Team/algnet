#pragma once
#include <queue>

#include "device.hpp"

namespace sim {

class Packet;

class ISender : public IRoutingDevice, IProcessingDevice {
public:
    virtual ~ISender() = default;
    virtual void enqueue_packet(Packet packet) = 0;
    virtual std::uint32_t send_data() = 0;
};

class Sender : public ISender {
public:
    Sender();
    ~Sender() = default;

    void add_inlink(std::shared_ptr<ILink> link) final;
    void update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<ILink> link) final;
    std::vector<std::shared_ptr<IRoutingDevice>> get_neighbours() const final;
    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> dest) const final;

    DeviceType get_type() const final;
    // Process an ACK by removing it from the ingress buffer,
    // update the flow state,
    // and schedule next ACK processing event after a delay.
    // Send the next packet: dequeue from the flow buffer
    // and move it to the egress port (link)
    // ACKs are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    std::uint32_t process() final;
    std::uint32_t send_data() final;

    void enqueue_packet(Packet packet) final;

private:
    std::queue<Packet> m_flow_buffer;
    std::unique_ptr<IRoutingDevice> m_router;
};

}  // namespace sim
