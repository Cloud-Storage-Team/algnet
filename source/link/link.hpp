#pragma once

#include <queue>

#include "event/event.hpp"
#include "link/interfaces/i_link.hpp"
#include "packet.hpp"

namespace sim {

class Link : public ILink, public std::enable_shared_from_this<Link> {
public:
    Link(Id a_id, std::weak_ptr<IRoutingDevice> a_from,
         std::weak_ptr<IRoutingDevice> a_to, std::uint32_t a_speed_gbps = 1,
         Time a_delay = 0, Size a_max_from_egress_buffer_size = 4096,
         Size a_max_to_ingress_buffer_size = 4096);
    ~Link() = default;

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    void schedule_arrival(Packet packet) final;

    std::optional<Packet> get_packet() final;

    std::shared_ptr<IRoutingDevice> get_from() const final;
    std::shared_ptr<IRoutingDevice> get_to() const final;

    Size get_from_egress_queue_size() const final;
    Size get_max_from_egress_buffer_size() const final;

    Size get_to_ingress_queue_size() const final;
    Size get_max_to_ingress_queue_size() const final;

    Id get_id() const final;

private:
    class Arrive : public Event {
    public:
        Arrive(Time a_time, std::weak_ptr<Link> a_link, Packet a_packet);
        void operator()() final;

    private:
        std::weak_ptr<Link> m_link;
        Packet m_paket;
    };

    // Removes packet from the source egress queue.
    void process_arrival(Packet packet);

    Time get_transmission_time(const Packet& packet) const;

    Id m_id;
    std::weak_ptr<IRoutingDevice> m_from;
    std::weak_ptr<IRoutingDevice> m_to;
    std::uint32_t m_speed_gbps;

    Size m_from_egress_queue_size;
    Size m_max_from_egress_buffer_size;
    Time m_arrival_time;

    Time m_transmission_delay;

    // Queue at the ingress port of the m_to device
    std::queue<Packet> m_to_ingress;
    // We keep track of m_to_ingress size in bytes
    // to account for packet size variations
    Size m_to_ingress_queue_size;
    Size m_max_to_ingress_buffer_size;
};

}  // namespace sim
