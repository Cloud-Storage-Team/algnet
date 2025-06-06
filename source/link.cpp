#include "link.hpp"

#include "device/device.hpp"
#include "event.hpp"
#include "logger/logger.hpp"
#include "metrics/metrics_collector.hpp"
#include "packet.hpp"
#include "scheduler.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

Link::Link(Id a_id, std::weak_ptr<IRoutingDevice> a_from,
           std::weak_ptr<IRoutingDevice> a_to, std::uint32_t a_speed_gbps,
           Time a_delay, Size a_max_from_egress_buffer_size,
           Size a_max_to_ingress_buffer_size)
    : m_id(a_id),
      m_from(a_from),
      m_to(a_to),
      m_speed_gbps(a_speed_gbps),
      m_from_egress_queue_size(0),
      m_max_from_egress_buffer_size(a_max_from_egress_buffer_size),
      m_arrival_time(0),
      m_transmission_delay(a_delay),
      m_to_ingress(),
      m_to_ingress_queue_size(0),
      m_max_to_ingress_buffer_size(a_max_to_ingress_buffer_size) {
    if (a_from.expired() || a_to.expired()) {
        LOG_WARN("Passed link to device is expired");
    } else if (a_speed_gbps == 0) {
        LOG_WARN("Passed zero link speed");
    }
}

Time Link::get_transmission_time(const Packet& packet) const {
    if (m_speed_gbps == 0) {
        LOG_WARN("Passed zero link speed");
        return 0;
    }
    const std::uint32_t byte_to_bit_multiplier = 8;

    Size packet_size_bit = packet.size_byte * byte_to_bit_multiplier;
    std::uint32_t transmission_speed_bit_ns = m_speed_gbps;
    return (packet_size_bit + transmission_speed_bit_ns - 1) /
               transmission_speed_bit_ns +
           m_transmission_delay;
};

void Link::schedule_arrival(Packet packet) {
    if (m_to.expired()) {
        LOG_WARN("Destination device pointer is expired");
        return;
    }

    if (m_from_egress_queue_size + packet.size_byte >
        m_max_from_egress_buffer_size) {
        LOG_ERROR("Egress buffer overflow; packet " + packet.to_string() +
                  " lost");
        return;
    }

    unsigned int transmission_time = get_transmission_time(packet);
    m_arrival_time =
        std::max(m_arrival_time, Scheduler::get_instance().get_current_time()) +
        transmission_time;

    m_from_egress_queue_size += packet.size_byte;
    MetricsCollector::get_instance().add_queue_size(
        get_id(), Scheduler::get_instance().get_current_time(),
        m_from_egress_queue_size);

    Scheduler::get_instance().add(
        std::make_unique<Arrive>(m_arrival_time, weak_from_this(), packet));
};

void Link::process_arrival(Packet packet) {
    if (m_to_ingress_queue_size + packet.size_byte >
        m_max_to_ingress_buffer_size) {
        LOG_ERROR("Ingress buffer overflow; packet " + packet.to_string() +
                  " lost");
        return;
    }

    // Remove packet from the source egress queue
    m_from_egress_queue_size -= packet.size_byte;

    MetricsCollector::get_instance().add_queue_size(
        get_id(), Scheduler::get_instance().get_current_time(),
        m_from_egress_queue_size);

    // Add packet to the next device ingress queue
    m_to_ingress.push(packet);
    m_to_ingress_queue_size += packet.size_byte;

    m_to.lock()->notify_about_arrival(
        Scheduler::get_instance().get_current_time());
    LOG_INFO("Packet arrived to the next device. Packet: " +
             packet.to_string());
};

std::optional<Packet> Link::get_packet() {
    if (m_to_ingress.empty()) {
        LOG_INFO("Ingress packet queue is empty");
        return {};
    }

    auto packet = m_to_ingress.front();
    LOG_INFO("Packet is taken from the link. Packet: " + packet.to_string());
    m_to_ingress.pop();
    m_to_ingress_queue_size -= packet.size_byte;
    return packet;
};

std::shared_ptr<IRoutingDevice> Link::get_from() const {
    if (m_from.expired()) {
        LOG_WARN("Source device pointer is expired");
        return nullptr;
    }

    return m_from.lock();
};

std::shared_ptr<IRoutingDevice> Link::get_to() const {
    if (m_to.expired()) {
        LOG_WARN("Destination device pointer is expired");
        return nullptr;
    }

    return m_to.lock();
};

Size Link::get_max_from_egress_buffer_size() const {
    return m_max_from_egress_buffer_size;
}

Id Link::get_id() const { return m_id; }

}  // namespace sim
