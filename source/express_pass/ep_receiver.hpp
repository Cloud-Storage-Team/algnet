#pragma once
#include <iostream>
#include <memory>

#include "device/receiver.hpp"

namespace sim {

class EPReceiver : public IReceiver,
                 public std::enable_shared_from_this<EPReceiver> {
public:
    EPReceiver();
    ~EPReceiver() {
        std::cout << "Receiver " << get_id() << " got " << m_cnt << " packets" << std::endl;
    };

    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool add_outlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(Id dest_id, std::shared_ptr<ILink> link, size_t paths_count = 1) final;
    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(Packet packet) const final;
    std::set<std::shared_ptr<ILink>> get_outlinks() final;

    DeviceType get_type() const final;
    // Process a packet by removing it from the ingress buffer
    // Send an ACK to the egress buffer
    // and schedule next receive event after a delay.
    // Upon receiving send an ACK to the sender.
    // Packets are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    Time process() final;
    Time send_system_packet(Packet packet);

    Id get_id() const final;

private:
    Time send_ack(Packet data_packet);
    std::unique_ptr<RoutingModule> m_router;
    int m_cnt = 0;
};

}  // namespace sim
