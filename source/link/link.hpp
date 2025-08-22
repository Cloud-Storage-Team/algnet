#pragma once

#include <queue>

#include "event/event.hpp"
#include "link/i_link.hpp"
#include "packet_queue/link_queue.hpp"
#include "utils/str_expected.hpp"

namespace sim {

struct LinkInitArgs {
    utils::str_expected<Id> id = std::unexpected("Missing id");
    utils::str_expected<Id> from_id = std::unexpected("Missing from id");
    utils::str_expected<Id> to_id = std::unexpected("Missing to id");
    utils::str_expected<SpeedGbps> speed = std::unexpected("Missing speed");
    utils::str_expected<TimeNs> delay = std::unexpected("Missing delay");
    utils::str_expected<SizeByte> max_from_egress_buffer_size =
        std::unexpected("Missing max from egress buffer size");
    utils::str_expected<SizeByte> max_to_ingress_buffer_size =
        std::unexpected("Missing max to ingress buffer size");
    ;
};

class Link : public ILink, public std::enable_shared_from_this<Link> {
public:
    Link(Id a_id, std::weak_ptr<IDevice> a_from, std::weak_ptr<IDevice> a_to,
         SpeedGbps a_speed = SpeedGbps(1), TimeNs a_delay = TimeNs(0),
         SizeByte a_max_from_egress_buffer_size = SizeByte(4096),
         SizeByte a_max_to_ingress_buffer_size = SizeByte(4096));
    explicit Link(LinkInitArgs args);
    ~Link() = default;

    void schedule_arrival(Packet packet) final;

    std::optional<Packet> get_packet() final;

    std::shared_ptr<IDevice> get_from() const final;
    std::shared_ptr<IDevice> get_to() const final;

    SizeByte get_from_egress_queue_size() const final;
    SizeByte get_max_from_egress_buffer_size() const final;

    SizeByte get_to_ingress_queue_size() const final;
    SizeByte get_max_to_ingress_queue_size() const final;

    Id get_id() const final;

private:
    class Transmit : public Event {
    public:
        Transmit(TimeNs a_time, std::weak_ptr<Link> a_link);
        void operator()() final;

    private:
        std::weak_ptr<Link> m_link;
    };

    class Arrive : public Event {
    public:
        Arrive(TimeNs a_time, std::weak_ptr<Link> a_link, Packet a_packet);
        void operator()() final;

    private:
        std::weak_ptr<Link> m_link;
        Packet m_paket;
    };

    // Head packet leaves source egress queue
    void transmit();

    // Packet arrives to destination ingress queue
    void arrive(Packet packet);

    TimeNs get_transmission_delay(const Packet& packet) const;

    // Shedule Transmit event
    void start_head_packet_sending();

    Id m_id;
    std::weak_ptr<IDevice> m_from;
    std::weak_ptr<IDevice> m_to;
    SpeedGbps m_speed;

    TimeNs m_propagation_delay;

    // Queue at the ingress port of the m_to device
    LinkQueue m_from_egress;

    // Queue at the egress port of the m_to device
    LinkQueue m_to_ingress;
};

}  // namespace sim
