#pragma once
#include "../../source/device.hpp"
#include "../../source/link.hpp"

class LinkMock : public sim::ILink {
public:
    LinkMock();
    void schedule_arrival(sim::Packet a_packet) final;
    sim::Packet get_packet() final;
    std::shared_ptr<sim::IRoutingDevice> get_from() const final;
    std::shared_ptr<sim::IRoutingDevice> get_to() const final;

    void set_ingress_packet(sim::Packet a_paket);
    std::vector<sim::Packet> get_arrived_packets() const;

private:
    std::vector<sim::Packet> m_arrived_packets;
    sim::Packet m_ingress_packet;
};
