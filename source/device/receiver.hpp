#pragma once

#include "event.hpp"
#include "device/scheduling_module.hpp"
#include "device/interfaces/i_receiver.hpp"

namespace sim {

class Receiver : public IReceiver,
                 public std::enable_shared_from_this<Receiver> {
public:
    Receiver(Id a_id);
    ~Receiver() = default;

    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool add_outlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(Id dest_id, std::shared_ptr<ILink> link, size_t paths_count = 1) final;
    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(Packet packet) const final;
    std::set<std::shared_ptr<ILink>> get_outlinks() final;

    bool notify_about_arrival(Time arrival_time) final;

    DeviceType get_type() const final;
    // Process a packet by removing it from the ingress buffer
    // Send an ACK to the egress buffer
    // and schedule next receive event after a delay.
    // Upon receiving send an ACK to the sender.
    // Packets are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    Time process() final;

    Id get_id() const final;

private:
    Time send_ack(Packet data_packet);
    std::unique_ptr<IRoutingDevice> m_router;
    SchedulingModule<IReceiver, Process> m_process_scheduler;
};

}  // namespace sim
