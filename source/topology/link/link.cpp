#include "topology/link/link.hpp"

#include <spdlog/fmt/fmt.h>

#include "logger/logger.hpp"
#include "scheduler/scheduler.hpp"
#include "utils/str_expected.hpp"

namespace sim {

std::shared_ptr<Link> Link::create_shared(
    Id a_id, std::weak_ptr<IDevice> a_from, std::weak_ptr<IDevice> a_to,
    SpeedGbps a_speed, TimeNs a_propagation_delay,
    SizeByte a_max_from_egress_buffer_size,
    SizeByte a_max_to_ingress_buffer_size,
    LinkMetricsFilters a_metrics_filters) {
    return std::shared_ptr<Link>(
        new Link(a_id, a_from, a_to, a_speed, a_propagation_delay,
                 a_max_from_egress_buffer_size, a_max_to_ingress_buffer_size,
                 a_metrics_filters));
}

void Link::schedule_arrival(const Packet& packet) {
    bool empty_before_push = m_from_egress.empty();

    if (!m_from_egress.push(packet)) {
        LOG_ERROR(
            fmt::format("Link {}: from egress buffer overflow; packet {} lost",
                        m_id, packet.to_string()));
        return;
    }

    if (empty_before_push) {
        start_head_packet_sending();
    }
};

std::optional<Packet> Link::get_packet() {
    if (m_to_ingress.empty()) {
        LOG_INFO("Ingress packet queue is empty");
        return {};
    }

    Packet packet = m_to_ingress.front();
    m_to_ingress.pop();
    return packet;
};

std::shared_ptr<IDevice> Link::get_from() const {
    if (m_from.expired()) {
        LOG_WARN("Source device pointer is expired");
        return nullptr;
    }

    return m_from.lock();
};

std::shared_ptr<IDevice> Link::get_to() const {
    if (m_to.expired()) {
        LOG_WARN("Destination device pointer is expired");
        return nullptr;
    }

    return m_to.lock();
};

SizeByte Link::get_from_egress_queue_size() const {
    return m_from_egress.get_size();
}

SizeByte Link::get_max_from_egress_buffer_size() const {
    return m_from_egress.get_max_size();
}

SizeByte Link::get_to_ingress_queue_size() const {
    return m_to_ingress.get_size();
}

SizeByte Link::get_max_to_ingress_queue_size() const {
    return m_to_ingress.get_max_size();
}

Id Link::get_id() const { return m_id; }

MetricsTable Link::get_metrics_table() const {
    MetricsTable result;
    if (m_metrics_filters.queues_size) {
        result.emplace("ingress queue size",
                       m_from_egress.get_queue_size_storage());
        result.emplace("egress queue size",
                       m_to_ingress.get_queue_size_storage());
    }
    return result;
}

void Link::write_inner_metrics(
    [[maybe_unused]] std::filesystem::path output_dir) const {}

Link::Link(Id a_id, std::weak_ptr<IDevice> a_from, std::weak_ptr<IDevice> a_to,
           SpeedGbps a_speed, TimeNs a_propagation_delay,
           SizeByte a_max_from_egress_buffer_size,
           SizeByte a_max_to_ingress_buffer_size,
           LinkMetricsFilters a_metrics_filters)
    : m_id(a_id),
      m_from(a_from),
      m_to(a_to),
      m_speed(a_speed),
      m_propagation_delay(a_propagation_delay),
      m_from_egress(a_max_from_egress_buffer_size, a_id,
                    LinkQueueType::FromEgress),
      m_to_ingress(a_max_to_ingress_buffer_size, a_id,
                   LinkQueueType::ToIngress),
      m_metrics_filters(a_metrics_filters) {
    if (a_from.expired() || a_to.expired()) {
        LOG_WARN("Passed link to device is expired");
    } else if (a_speed == SpeedGbps(0)) {
        LOG_WARN("Passed zero link speed");
    }
}

class Link::Transmit : public Event {
public:
    Transmit(TimeNs a_time, std::shared_ptr<Link> a_link)
        : Event(a_time), m_link(a_link) {}
    void operator()() final { m_link->transmit(); }

private:
    std::shared_ptr<Link> m_link;
};

class Link::Arrive : public Event {
public:
    Arrive(TimeNs a_time, std::shared_ptr<Link> a_link, const Packet& a_packet)
        : Event(a_time), m_link(a_link), m_packet(a_packet) {}
    void operator()() final { m_link->arrive(m_packet); }

private:
    std::shared_ptr<Link> m_link;
    Packet m_packet;
};

TimeNs Link::get_transmission_delay(const Packet& packet) const {
    if (m_speed == SpeedGbps(0)) {
        LOG_WARN("Passed zero link speed");
        return TimeNs(0);
    }
    return packet.size / m_speed;
};

void Link::transmit() {
    if (m_from_egress.empty()) {
        LOG_ERROR(fmt::format(
            "Transmit on link {} with empty source egress buffer", m_id));
        return;
    }
    TimeNs current_time = Scheduler::get_instance().get_current_time();
    Scheduler::get_instance().add<Arrive>(current_time + m_propagation_delay,
                                          shared_from_this(),
                                          m_from_egress.front());
    m_from_egress.pop();
    if (!m_from_egress.empty()) {
        start_head_packet_sending();
    }
}

void Link::arrive(const Packet& packet) {
    if (!m_to_ingress.push(packet)) {
        LOG_ERROR(
            fmt::format("Ingress buffer on link {} overflow; packet {} lost",
                        m_id, packet.to_string()));
        return;
    }

    m_to.lock()->notify_about_arrival();
    LOG_INFO("Packet arrived to the next device. Packet: " +
             packet.to_string());
};

void Link::start_head_packet_sending() {
    TimeNs current_time = Scheduler::get_instance().get_current_time();
    Scheduler::get_instance().add<Transmit>(
        current_time + get_transmission_delay(m_from_egress.front()),
        shared_from_this());
}

}  // namespace sim
