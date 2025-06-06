#include "express_pass/ep_link.hpp"

#include "device/switch.hpp"
#include "scheduler.hpp"
#include "logger/logger.hpp"

namespace sim {

EPLink::EPLink(Id a_id, std::weak_ptr<IRoutingDevice> a_from,
           std::weak_ptr<IRoutingDevice> a_to, std::uint32_t a_speed_gbps,
           Time a_delay, Size a_max_src_egress_buffer_size_byte,
           Size a_max_ingress_buffer_size)
    : m_id(a_id),
      m_from(a_from),
      m_to(a_to),
      m_speed_gbps(a_speed_gbps),
      m_src_egress_buffer_size_byte(0),
      m_max_src_egress_buffer_size_byte(a_max_src_egress_buffer_size_byte),
      m_last_src_egress_pass_time(0),
      m_transmission_delay(a_delay),
      m_next_ingress(),
      m_ingress_buffer_size_byte(0),
      m_max_ingress_buffer_size_byte(a_max_ingress_buffer_size) {
    if (a_from.expired() || a_to.expired()) {
        LOG_WARN("Passed link to device is expired");
    } else if (a_speed_gbps == 0) {
        LOG_WARN("Passed zero link speed");
    }
}

Time EPLink::get_transmission_time(const Packet& packet) const {
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

void EPLink::update_token_bucket() {
    Time passed = Scheduler::get_instance().get_current_time() - m_last_token_update;
    Size tokens_income = passed * (double)m_speed_gbps * m_token_getting_speed * 8;
    
    m_token_bucket_size = std::min(m_token_bucket_size + tokens_income, m_max_token_bucket_size);

    m_last_token_update = Scheduler::get_instance().get_current_time();
}

void EPLink::schedule_arrival(Packet packet) {
    if (m_to.expired()) {
        LOG_WARN("Destination device pointer is expired");
        return;
    }
    
    update_token_bucket();

    Time total_delay;

    // TODO: rewrite this part with limiting throughput instead of calculating sending time separately
    auto src = m_from.lock();
    if (packet.type == PacketType::CREDIT) {
        if (m_current_credit_queue_capacity + packet.size_byte > m_max_credit_queue_capacity) {
            // LOG_ERROR("Dropped credit: " + packet.to_string() + " Time: " + std::to_string(Scheduler::get_instance().get_current_time()));
            return;
        }

        if (packet.size_byte > m_token_bucket_size) {
            return;
        }
        m_token_bucket_size -= packet.size_byte;

        m_current_credit_queue_capacity += packet.size_byte;
        total_delay = std::max(m_next_credit_can_be_sent, Scheduler::get_instance().get_current_time());
        m_next_credit_can_be_sent = total_delay + (packet.size_byte * 8 + 1538 * 8) / m_speed_gbps;
        // LOG_ERROR("Next credit can be sent: " + std::to_string(m_next_credit_can_be_sent));
    } else {
        if (m_src_egress_buffer_size_byte + packet.size_byte >
            m_max_src_egress_buffer_size_byte) {
            LOG_ERROR("Buffer in link overflowed; packet " + packet.to_string() +
                      " lost");
            return;
        }

        LOG_INFO("Packet arrived to link's ingress queue. Packet: " +
             packet.to_string());
        Time transmission_time = get_transmission_time(packet);

        m_last_src_egress_pass_time =
            std::max(m_last_src_egress_pass_time,
                     Scheduler::get_instance().get_current_time()) +
            transmission_time;
        m_src_egress_buffer_size_byte += packet.size_byte;
        m_max_buffer_size = std::max(m_max_buffer_size, m_src_egress_buffer_size_byte);
        total_delay = m_last_src_egress_pass_time;
    }

    m_to.lock()->notify_about_arrival(
        Scheduler::get_instance().get_current_time());

    Scheduler::get_instance().add(std::make_unique<Arrive>(
        Arrive(total_delay, weak_from_this(), packet)));
};

void EPLink::process_arrival(Packet packet) {
    auto src = m_from.lock();
    if (packet.type == PacketType::CREDIT) {
        m_current_credit_queue_capacity -= packet.size_byte;
    } else {    
        if (m_ingress_buffer_size_byte + packet.size_byte >
            m_max_ingress_buffer_size_byte) {
            LOG_ERROR("Ingress buffer on EPLink overflow; packet" +
                  packet.to_string() + " lost");
            return;
        }
        m_ingress_buffer_size_byte += packet.size_byte;
        m_max_buffer_size = std::max(m_max_buffer_size, m_ingress_buffer_size_byte);

        LOG_INFO("Packet arrived to link's egress queue. Packet: " +
             packet.to_string());

        m_src_egress_buffer_size_byte -= packet.size_byte;
    }
    m_next_ingress.push(packet);
};

std::optional<Packet> EPLink::get_packet() {
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

std::shared_ptr<IRoutingDevice> EPLink::get_from() const {
    if (m_from.expired()) {
        LOG_WARN("Source device pointer is expired");
        return nullptr;
    }

    return m_from.lock();
};

std::shared_ptr<IRoutingDevice> EPLink::get_to() const {
    if (m_to.expired()) {
        LOG_WARN("Destination device pointer is expired");
        return nullptr;
    }

    return m_to.lock();
};

Size EPLink::get_max_from_egress_buffer_size() const {
    return m_max_src_egress_buffer_size_byte;
}

Id EPLink::get_id() const { return m_id; }

}  // namespace sim
