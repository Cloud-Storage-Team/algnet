#pragma once

#include <queue>

#include "event/event.hpp"
#include "link/interfaces/i_link.hpp"
#include "packet_queue.hpp"

namespace sim {

class Link : public ILink, public std::enable_shared_from_this<Link> {
public:
    Link(Id a_id, std::weak_ptr<IRoutingDevice> a_from,
         std::weak_ptr<IRoutingDevice> a_to, std::uint32_t a_speed_gbps = 1,
         Time a_delay = 0, Size a_max_from_egress_buffer_size = 4096,
         Size a_max_to_ingress_buffer_size = 4096);
    ~Link() = default;

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
    class Transmit : public Event {
    public:
        Transmit(Time a_time, std::weak_ptr<Link> a_link);
        void operator()() final;

    private:
        std::weak_ptr<Link> m_link;
    };

    class Arrive : public Event {
    public:
        Arrive(Time a_time, std::weak_ptr<Link> a_link, Packet a_packet);
        void operator()() final;

    private:
        std::weak_ptr<Link> m_link;
        Packet m_paket;
    };

    // Head packet leaves source egress queue
    void transmit();

    // Packet arrives to destination ingress queue
    void arrive(Packet packet);

    Time get_transmission_delay(const Packet& packet) const;

    // Shedule Transmit event
    void start_head_packet_sending();

    Id m_id;
    std::weak_ptr<IRoutingDevice> m_from;
    std::weak_ptr<IRoutingDevice> m_to;
    std::uint32_t m_speed_gbps;

    Time m_propagation_delay;

    // Queue at the ingress port of the m_to device
    PacketQueue m_from_egress;

    // Queue at the egress port of the m_to device
    PacketQueue m_to_ingress;
};

}  // namespace sim
