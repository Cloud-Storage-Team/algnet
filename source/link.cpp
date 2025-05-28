#include "link.hpp"

#include "device/device.hpp"
#include "event.hpp"
#include "logger/logger.hpp"
#include "scheduler.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

Link::Link(std::weak_ptr<IRoutingDevice> a_from,
           std::weak_ptr<IRoutingDevice> a_to, std::uint32_t a_speed_gbps,
           Time a_delay, Size a_max_src_egress_buffer_size_byte,
           Size a_max_ingress_buffer_size)
    : m_from(a_from),
      m_to(a_to),
      m_speed_gbps(a_speed_gbps),
      m_src_egress_buffer_size_byte(0),
      m_max_src_egress_buffer_size_byte(a_max_src_egress_buffer_size_byte),
      m_last_src_egress_pass_time(0),
      m_transmission_delay(a_delay),
      m_id(IdentifierFactory::get_instance().generate_id()),
      m_next_ingress(),
      m_ingress_buffer_size_byte(0),
      m_max_ingress_buffer_size_byte(a_max_ingress_buffer_size) {
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

    if (m_src_egress_buffer_size_byte + packet.size_byte >
        m_max_src_egress_buffer_size_byte) {
        LOG_ERROR("Buffer in link overflowed; packet " + packet.to_string() +
                  " lost");
        return;
    }

    LOG_INFO("Packet arrived to link's ingress queue. Packet: " +
             packet.to_string());

    unsigned int transmission_time = get_transmission_time(packet);
    m_last_src_egress_pass_time =
        std::max(m_last_src_egress_pass_time,
                 Scheduler::get_instance().get_current_time()) +
        transmission_time;
    m_src_egress_buffer_size_byte += packet.size_byte;

    Scheduler::get_instance().add(
        std::make_unique<Arrive>(m_last_src_egress_pass_time, weak_from_this(), packet));
};

void Link::process_arrival(Packet packet) {
    if (m_ingress_buffer_size_byte + packet.size_byte >
        m_max_ingress_buffer_size_byte) {
        LOG_ERROR("Ingress buffer on Link overflow; packet" +
                  packet.to_string() + " lost");
        return;
    }
    m_ingress_buffer_size_byte += packet.size_byte;

    LOG_INFO("Packet arrived to link's egress queue. Packet: " +
             packet.to_string());

    m_to.lock()->notify_about_arrival(Scheduler::get_instance().get_current_time());

    m_src_egress_buffer_size_byte -= packet.size_byte;
    m_next_ingress.push(packet);
};

std::optional<Packet> Link::get_packet() {
    if (m_next_ingress.empty()) {
        LOG_INFO("Ingress packet queue is empty");
        return {};
    }

    auto packet = m_next_ingress.front();
    LOG_INFO("Taken packet from link. Packet: " + packet.to_string());
    m_next_ingress.pop();
    m_ingress_buffer_size_byte -= packet.size_byte;
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

Id Link::get_id() const { return m_id; }

}  // namespace sim
