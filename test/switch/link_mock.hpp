#pragma once
#include "../../source/device.hpp"
#include "../../source/link.hpp"

class LinkMock : public sim::Link {
public:
    LinkMock(sim::Device* a_src, sim::Device* a_dest);
    ~LinkMock() = default;

    void set_ingress_packet(sim::Packet);
    sim::Packet get_packet() final;
    void schedule_arrival(sim::Packet a_packet) final;

    std::vector<sim::Packet> get_arrived_packets() const;

private:
    std::vector<sim::Packet> m_arrived_packets;
    sim::Packet m_ingress_packet;
};
