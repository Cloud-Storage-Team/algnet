#include "link_mock.hpp"

LinkMock::LinkMock(std::weak_ptr<sim::IDevice> a_from,
                   std::weak_ptr<sim::IDevice> a_to)
    : m_from(a_from), m_to(a_to), m_arrived_packets(), m_ingress_packet() {}

std::shared_ptr<sim::IDevice> LinkMock::get_from() const {
    return m_from.lock();
}
std::shared_ptr<sim::IDevice> LinkMock::get_to() const { return m_to.lock(); }

void LinkMock::schedule_arrival(sim::Packet a_packet) {
    m_arrived_packets.push_back(a_packet);
}

void LinkMock::process_arrival([[maybe_unused]] sim::Packet a_packet) {}

void LinkMock::set_ingress_packet(sim::Packet a_packet) {
    m_ingress_packet.emplace(a_packet);
}

std::optional<sim::Packet> LinkMock::get_packet() { return m_ingress_packet; }

std::vector<sim::Packet> LinkMock::get_arrived_packets() const {
    return m_arrived_packets;
}

SizeByte LinkMock::get_from_egress_queue_size() const { return SizeByte(0); }
SizeByte LinkMock::get_max_from_egress_buffer_size() const {
    return SizeByte(4096);
}

SizeByte LinkMock::get_to_ingress_queue_size() const { return SizeByte(0); }
SizeByte LinkMock::get_max_to_ingress_queue_size() const { return SizeByte(0); }

Id LinkMock::get_id() const { return ""; }
