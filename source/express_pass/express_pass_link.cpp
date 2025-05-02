#include "express_pass_link.hpp"

#include "event.hpp"
#include "logger/logger.hpp"
#include "scheduler.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

ExpressPassLink::ExpressPassLink(std::weak_ptr<IRoutingDevice> a_from,
           std::weak_ptr<IRoutingDevice> a_to, std::uint32_t a_speed_mbps,
           Time a_delay)
    : m_from(a_from),
      m_to(a_to),
      m_speed_mbps(a_speed_mbps),
      m_src_egress_delay(0),
      m_transmission_delay(a_delay),
      m_id(IdentifierFactory::get_instance().generate_id()) {
    if (a_from.expired() || a_to.expired()) {
        LOG_WARN("Passed link to device is expired");
    } else if (a_speed_mbps == 0) {
        LOG_WARN("Passed zero link speed");
    }
}

Time ExpressPassLink::get_transmission_time(const Packet& packet) const {
    if (m_speed_mbps == 0) {
        LOG_WARN("Passed zero link speed");
        return 0;
    }
    return (packet.size + m_speed_mbps - 1) / m_speed_mbps +
           m_transmission_delay;
};

void ExpressPassLink::schedule_arrival(Time current_time, Packet packet) {
    if (m_to.expired()) {
        LOG_WARN("Destination device pointer is expired");
        return;
    }

    LOG_INFO("Packet arrived to link's ingress queue. Packet: " + packet.to_string() + ". Link id: " + std::to_string(get_id()) + ". Time: " + std::to_string(current_time));

    unsigned int transmission_time = get_transmission_time(packet);
    unsigned int total_delay;

    if (packet.type == PacketType::CREDIT) {
        if (m_current_credit_queue_capacity >= m_max_credit_queue_capacity) {
            LOG_INFO("Dropped credit: " + packet.to_string());
            return;
        }

        m_current_credit_queue_capacity++;
        total_delay = std::max(m_next_credit_can_be_sent, current_time);
        // TODO: make sure to calculate speed in GBe
        m_next_credit_can_be_sent = total_delay + (packet.size + 1538) / m_speed_mbps;
        LOG_INFO("Next credit can be sent: " + std::to_string(m_next_credit_can_be_sent));

    } else {
        m_src_egress_delay += transmission_time;
        total_delay = current_time + m_src_egress_delay;
    }

    Scheduler::get_instance().add(
        std::make_unique<Arrive>(Arrive(total_delay, weak_from_this(), packet)));
};

void ExpressPassLink::process_arrival(Packet packet) {
    LOG_INFO("Packet arrived to link's egress queue. Packet: " + packet.to_string() + ". Link id: " + std::to_string(get_id()));
    if (packet.type == PacketType::CREDIT) {
        m_current_credit_queue_capacity--;
    } else {
        m_src_egress_delay -= get_transmission_time(packet);
    }
    m_next_ingress.push(packet);
};

std::optional<Packet> ExpressPassLink::get_packet() {
    if (m_next_ingress.empty()) {
        LOG_TRACE("Ingress packet queue is empty");
        return {};
    }

    auto packet = m_next_ingress.front();
    LOG_INFO("Taken packet from link. Packet: " + packet.to_string() + ". Link id: " + std::to_string(get_id()));
    m_next_ingress.pop();
    return packet;
};

std::shared_ptr<IRoutingDevice> ExpressPassLink::get_from() const {
    if (m_from.expired()) {
        LOG_WARN("Source device pointer is expired");
        return nullptr;
    }

    return m_from.lock();
};

std::shared_ptr<IRoutingDevice> ExpressPassLink::get_to() const {
    if (m_to.expired()) {
        LOG_WARN("Destination device pointer is expired");
        return nullptr;
    }

    return m_to.lock();
};

Id ExpressPassLink::get_id() const { return m_id; }

}  // namespace sim
