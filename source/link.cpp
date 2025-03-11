#include "link.hpp"

#include "event.hpp"
#include "scheduler.hpp"

namespace sim {

Link::Link(std::weak_ptr<IRoutingDevice> a_from,
           std::weak_ptr<IRoutingDevice> a_to, std::uint32_t a_speed_mbps,
           std::uint32_t a_delay)
    : m_from(a_from),
      m_to(a_to),
      m_speed_mbps(a_speed_mbps),
      m_transmission_delay(a_delay),
      m_src_egress_delay(0) {
    if (a_from.expired() || a_to.expired()) {
        // TODO: warning log
    } else if (a_speed_mbps == 0) {
        // TODO: warning log
    }
}

std::uint32_t Link::get_transmission_time(const Packet& packet) const {
    if (m_speed_mbps == 0) {
        // TODO: warning log
        return 0;
    }
    return (packet.size + m_speed_mbps - 1) / m_speed_mbps +
           m_transmission_delay;
};

void Link::schedule_arrival(Packet packet) {
    if (m_to.expired()) {
        // TODO: warning log
        return;
    }

    unsigned int transmission_time = get_transmission_time(packet);
    unsigned int total_delay = m_src_egress_delay + transmission_time;

    m_src_egress_delay += transmission_time;

    Scheduler::get_instance().add(std::make_unique<Arrive>(Arrive(this, new Packet(packet))));
};

void Link::process_arrival(Packet packet) {
    m_src_egress_delay -= get_transmission_time(packet);
    m_next_ingress.push(packet);
};

std::optional<Packet> Link::get_packet() {
    if (m_next_ingress.empty()) {
        // TODO: info log
        return {};
    }

    auto packet = m_next_ingress.front();
    m_next_ingress.pop();
    return packet;
};

std::shared_ptr<IRoutingDevice> Link::get_from() const {
    if (m_from.expired()) {
        // TODO: warning log
        return nullptr;
    }

    return m_from.lock();
};

std::shared_ptr<IRoutingDevice> Link::get_to() const {
    if (m_to.expired()) {
        // TODO: warning log
        return nullptr;
    }

    return m_to.lock();
};

}  // namespace sim
