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

    static std::shared_ptr<Link> create_shared(
        Id a_id, std::weak_ptr<IDevice> a_from, std::weak_ptr<IDevice> a_to,
        SpeedGbps a_speed = SpeedGbps(1), TimeNs a_delay = TimeNs(0),
        SizeByte a_max_from_egress_buffer_size = SizeByte(4096),
        SizeByte a_max_to_ingress_buffer_size = SizeByte(4096),
        LinkMetricsFilters a_metrics_filters = DEFAULT_METRICS_FILTERS);

    virtual void schedule_arrival(const Packet& packet) final;

    virtual bool has_packet() const final;
    virtual Packet& get_packet() final;
    virtual void pop_packet() final;

    virtual std::shared_ptr<IDevice> get_from() const final;
    virtual std::shared_ptr<IDevice> get_to() const final;

    virtual SizeByte get_from_egress_queue_size() const final;
    virtual SizeByte get_max_from_egress_buffer_size() const final;

    virtual SizeByte get_to_ingress_queue_size() const final;
    virtual SizeByte get_max_to_ingress_queue_size() const final;

    virtual const Id& get_id() const final;

    virtual MetricsTable get_metrics_table() const final;

    virtual void write_inner_metrics(
        std::filesystem::path output_dir) const final;

private:
    Link(Id a_id, std::weak_ptr<IDevice> a_from, std::weak_ptr<IDevice> a_to,
         SpeedGbps a_speed, TimeNs a_delay,
         SizeByte a_max_from_egress_buffer_size,
         SizeByte a_max_to_ingress_buffer_size,
         LinkMetricsFilters a_metrics_filters);

private:
    // Head packet leaves source egress queue
    void transmit();

    // Packet arrives to destination ingress queue
    void arrive(const Packet& packet);

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
