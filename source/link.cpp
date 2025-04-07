#include "link.hpp"

#include <spdlog/spdlog.h>

#include "event.hpp"
#include "scheduler.hpp"

namespace sim {

Link::Link(std::weak_ptr<IRoutingDevice> a_from,
           std::weak_ptr<IRoutingDevice> a_to, std::uint32_t a_speed_mbps,
           std::uint32_t a_delay)
    : m_from(a_from),
      m_to(a_to),
      m_speed_mbps(a_speed_mbps),
      m_src_egress_delay(0),
      m_transmission_delay(a_delay) {
    if (a_from.expired() || a_to.expired()) {
        spdlog::warn("Passed link to device is expired");
    } else if (a_speed_mbps == 0) {
        spdlog::warn("Passed zero link speed");
    }
}

std::uint32_t Link::get_transmission_time(const Packet& packet) const {
    if (m_speed_mbps == 0) {
        spdlog::warn("Passed zero link speed");
        return 0;
    }
    return (packet.size + m_speed_mbps - 1) / m_speed_mbps +
           m_transmission_delay;
};

void Link::schedule_arrival(Packet packet) {
    if (m_to.expired()) {
        spdlog::warn("Destination device pointer is expired");
        return;
    }

    unsigned int transmission_time = get_transmission_time(packet);
    unsigned int total_delay = m_src_egress_delay + transmission_time;
    (void) total_delay; // unused variable stub

    m_src_egress_delay += transmission_time;

    Scheduler::get_instance().add(
        std::make_unique<Arrive>(Arrive(this, new Packet(packet))));
};

void Link::process_arrival(Packet packet) {
    m_src_egress_delay -= get_transmission_time(packet);
    m_next_ingress.push(packet);
};

std::optional<Packet> Link::get_packet() {
    if (m_next_ingress.empty()) {
        spdlog::info("Ingress packet queue is empty");
        return {};
    }

    auto packet = m_next_ingress.front();
    m_next_ingress.pop();
    return packet;
};

std::shared_ptr<IRoutingDevice> Link::get_from() const {
    if (m_from.expired()) {
        spdlog::warn("Source device pointer is expired");
        return nullptr;
    }

    return m_from.lock();
};

std::shared_ptr<IRoutingDevice> Link::get_to() const {
    if (m_to.expired()) {
        spdlog::warn("Destination device pointer is expired");
        return nullptr;
    }

    return m_to.lock();
};

}  // namespace sim
