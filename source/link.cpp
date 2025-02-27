#include "link.hpp"

namespace sim {

Link::Link(Device* a_from, Device* a_to, std::uint32_t a_speed_mbps, std::uint32_t a_delay):
    m_from(a_from),
    m_to(a_to),
    m_speed_mbps(a_speed_mbps),
    m_src_egress_delay(a_delay),
    m_transmission_delay(0),
    m_next_ingress(std::queue<Packet>())
{}

Packet Link::get_packet() {
    return Packet(PacketType::DATA, 0, nullptr);
}

void Link::schedule_arrival(Packet a_packet) {}

}  // namespace sim
