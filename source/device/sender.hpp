#pragma once

#include <queue>

#include "event.hpp"
#include "device/scheduling_module.hpp"
#include "device/interfaces/i_sender.hpp"

namespace sim {

class Sender : public ISender,
               public std::enable_shared_from_this<Sender> {
public:
    Sender(Id a_id);
    ~Sender() = default;

    bool add_inlink(std::shared_ptr<ILink> link) final;
    bool add_outlink(std::shared_ptr<ILink> link) final;
    bool update_routing_table(Id dest_id, std::shared_ptr<ILink> link, size_t paths_count = 1) final;
    std::shared_ptr<ILink> next_inlink() final;
    std::shared_ptr<ILink> get_link_to_destination(Packet packet) const final;
    std::set<std::shared_ptr<ILink>> get_outlinks() final;
    bool notify_about_arrival(Time arrive_time) final;

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

    Id get_id() const final;

private:
    std::queue<Packet> m_flow_buffer;
    std::unique_ptr<IRoutingDevice> m_router;
    SchedulingModule<ISender, Process> m_process_scheduler;
    SchedulingModule<ISender, SendData> m_send_data_scheduler;
};

}  // namespace sim
