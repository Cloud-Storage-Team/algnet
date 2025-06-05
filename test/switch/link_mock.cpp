#include "link_mock.hpp"

LinkMock::LinkMock(std::weak_ptr<sim::IRoutingDevice> a_from,
                   std::weak_ptr<sim::IRoutingDevice> a_to)
    : m_from(a_from), m_to(a_to), m_arrived_packets(), m_ingress_packet() {}

std::shared_ptr<sim::IRoutingDevice> LinkMock::get_from() const {
    return m_from.lock();
}
std::shared_ptr<sim::IRoutingDevice> LinkMock::get_to() const {
    return m_to.lock();
}

void LinkMock::schedule_arrival(sim::Packet a_packet) {
    m_arrived_packets.push_back(a_packet);
}

void LinkMock::process_arrival(sim::Packet a_packet) {}

void LinkMock::set_ingress_packet(sim::Packet a_paket) {
    m_ingress_packet = a_paket;
}

std::optional<sim::Packet> LinkMock::get_packet() { return m_ingress_packet; }

std::vector<sim::Packet> LinkMock::get_arrived_packets() const {
    return m_arrived_packets;
}

Size LinkMock::get_current_from_egress_buffer_size() const { return 0; }

Size LinkMock::get_max_from_egress_buffer_size() const { return 4096; }

Id LinkMock::get_id() const { return ""; }
