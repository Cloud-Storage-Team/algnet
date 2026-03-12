#pragma once

#include <queue>

#include "metrics/metrics_table/i_metricable.hpp"
#include "packet_queue/link_queue.hpp"
#include "scheduler/event/event.hpp"
#include "topology/link/i_link.hpp"
#include "utils/str_expected.hpp"

namespace sim {

struct LinkMetricsFilters {
    bool queues_size = true;
};

class Link : public ILink, public std::enable_shared_from_this<Link> {
public:
    static constexpr inline LinkMetricsFilters DEFAULT_METRICS_FILTERS = {};

    Link(Id a_id, std::weak_ptr<IDevice> a_from, std::weak_ptr<IDevice> a_to,
         SpeedGbps a_speed = SpeedGbps(1), TimeNs a_delay = TimeNs(0),
         SizeByte a_max_from_egress_buffer_size = SizeByte(4096),
         SizeByte a_max_to_ingress_buffer_size = SizeByte(4096),
         LinkMetricsFilters a_metrics_filters = DEFAULT_METRICS_FILTERS);
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

    virtual MetricsTable get_metrics_table() const final;

    virtual void write_inner_metrics(
        std::filesystem::path output_dir) const final;

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

private:
    // Head packet leaves source egress queue
    void transmit();

    // Packet arrives to destination ingress queue
    void arrive(Packet packet);

    TimeNs get_transmission_delay(const Packet& packet) const;

    // Schedule Transmit event
    void start_head_packet_sending();

private:
    Id m_id;
    std::weak_ptr<IDevice> m_from;
    std::weak_ptr<IDevice> m_to;
    SpeedGbps m_speed;

    TimeNs m_propagation_delay;

    // Queue at the egress port of the m_from device
    LinkQueue m_from_egress;

    // Queue at the ingress port of the m_to device
    LinkQueue m_to_ingress;
    LinkMetricsFilters m_metrics_filters;
};

}  // namespace sim
