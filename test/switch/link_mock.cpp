#include "link_mock.hpp"

LinkMock::LinkMock(sim::Device* a_src, sim::Device* a_dest)
    : Link(a_src, a_dest, 0, 0) {}

void LinkMock::schedule_arrival(sim::Packet a_packet) {
    m_arrived_packets.push_back(a_packet);
}

void LinkMock::set_ingress_packet(sim::Packet a_paket) {
    m_ingress_packet = a_paket;
}

sim::Packet LinkMock::get_packet() { return m_ingress_packet; }

std::vector<sim::Packet> LinkMock::get_arrived_packets() const {
    return m_arrived_packets;
}
