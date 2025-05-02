#pragma once
#include <memory>

#include "device/receiver.hpp"

namespace sim {

struct Packet;

class ExpressPassReceiver : public IReceiver,
                 public std::enable_shared_from_this<ExpressPassReceiver> {
public:
    ExpressPassReceiver();
    ~ExpressPassReceiver() = default;

    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool add_outlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<ILink> link, int paths = 1) final;
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
    Time process(Time current_time) final;
    Time send_system_packet(Time current_time, Packet packet);

    Id get_id() const final;

private:
    std::unique_ptr<RoutingModule> m_router;
    Id m_id;
};

}  // namespace sim
