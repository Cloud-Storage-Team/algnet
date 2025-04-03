#pragma once
#include "device.hpp"
#include "link.hpp"

namespace test_receiver {

class LinkMock : public sim::ILink {
public:
    LinkMock(std::weak_ptr<sim::IRoutingDevice> a_from,
             std::weak_ptr<sim::IRoutingDevice> a_to)
        : m_from(a_from), m_to(a_to), m_arrived_packets(), m_ingress_packet() {}

    ~LinkMock() = default;

    std::shared_ptr<sim::IRoutingDevice> get_from() const {
        return m_from.lock();
    }
    std::shared_ptr<sim::IRoutingDevice> get_to() const { return m_to.lock(); }

    void schedule_arrival(sim::Packet a_packet) {
        m_arrived_packets.push_back(a_packet);
    }

    void process_arrival(sim::Packet a_packet) {}

    void set_ingress_packet(sim::Packet a_paket) { m_ingress_packet = a_paket; }

    std::optional<sim::Packet> get_packet() { return m_ingress_packet; }

    std::vector<sim::Packet> get_arrived_packets() const {
        return m_arrived_packets;
    }

private:
    std::weak_ptr<sim::IRoutingDevice> m_from;
    std::weak_ptr<sim::IRoutingDevice> m_to;
    std::vector<sim::Packet> m_arrived_packets;
    std::optional<sim::Packet> m_ingress_packet;
};

}  // namespace test_receiver