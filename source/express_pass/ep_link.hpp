#pragma once

#include "link.hpp"
#include <iostream>

namespace sim {

class EPLink : public ILink, public std::enable_shared_from_this<EPLink> {
public:
    EPLink(std::weak_ptr<IRoutingDevice> a_from,
         std::weak_ptr<IRoutingDevice> a_to, std::uint32_t a_speed_gbps = 1,
         Time a_delay = 0, Size a_max_src_egress_buffer_size_byte = 32768,
         Size a_max_ingress_buffer_size_byte = 32768);
    ~EPLink() {
        std::cout << "Max buffer: " << m_max_buffer_size << std::endl;
    };

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    void schedule_arrival(Packet packet) final;

    /**
     * Removes packet from the source egress queue.
     */
    void process_arrival(Packet packet) final;

    std::optional<Packet> get_packet() final;

    std::shared_ptr<IRoutingDevice> get_from() const final;
    std::shared_ptr<IRoutingDevice> get_to() const final;

    Id get_id() const final;

private:
    Time get_transmission_time(const Packet& packet) const;

    std::weak_ptr<IRoutingDevice> m_from;
    std::weak_ptr<IRoutingDevice> m_to;
    std::uint32_t m_speed_gbps;

    Size m_src_egress_buffer_size_byte;
    Size m_max_src_egress_buffer_size_byte;
    Time m_last_src_egress_pass_time;

    Time m_transmission_delay;
    Id m_id;

    // Queue at the ingress port of the m_next device
    std::queue<Packet> m_next_ingress;
    Size m_ingress_buffer_size_byte;
    Size m_max_ingress_buffer_size_byte;


    std::uint32_t m_max_credit_queue_capacity = 8;
    std::uint32_t m_current_credit_queue_capacity = 0;
    Size m_max_buffer_size = 0;
    Time m_next_credit_can_be_sent = 0;
};

}  // namespace sim
