#pragma once
#include <queue>

#include "sender.hpp"

namespace sim {

struct Packet;

class ExpressPassSender : public ISender, public std::enable_shared_from_this<Sender> {
public:
    ExpressPassSender();
    ~ExpressPassSender() = default;

    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool add_outlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<ILink> link) final;
    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> dest) const final;
    std::set<std::shared_ptr<ILink>> get_outlinks() const final;

    DeviceType get_type() const final;
    // Process an ACK by removing it from the ingress buffer,
    // update the flow state,
    // and schedule next ACK processing event after a delay.
    // Send the next packet: dequeue from the flow buffer
    // and move it to the egress port (link)
    // ACKs are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    Time process() final;
    Time send_data() final;

    void enqueue_packet(Packet packet) final;

private:
    std::queue<Packet> m_flow_buffer;
    std::unique_ptr<IRoutingDevice> m_router;
};

}  // namespace sim
