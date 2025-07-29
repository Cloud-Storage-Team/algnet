#pragma once

#include <vector>

#include "link/i_link.hpp"
#include "packet/packet.hpp"

class LinkMock : public sim::ILink {
public:
    LinkMock(std::weak_ptr<sim::IDevice> a_from,
             std::weak_ptr<sim::IDevice> a_to);
    ~LinkMock() = default;
    virtual void schedule_arrival(sim::Packet a_packet) final;
    virtual void process_arrival(sim::Packet packet) final;
    virtual std::optional<sim::Packet> get_packet() final;
    virtual std::shared_ptr<sim::IDevice> get_from() const final;
    virtual std::shared_ptr<sim::IDevice> get_to() const final;

    virtual SizeByte get_from_egress_queue_size() const final;
    virtual SizeByte get_max_from_egress_buffer_size() const final;

    virtual SizeByte get_to_ingress_queue_size() const final;
    virtual SizeByte get_max_to_ingress_queue_size() const final;

    void set_ingress_packet(sim::Packet a_paket);
    std::vector<sim::Packet> get_arrived_packets() const;

    Id get_id() const final;

private:
    std::weak_ptr<sim::IDevice> m_from;
    std::weak_ptr<sim::IDevice> m_to;
    std::vector<sim::Packet> m_arrived_packets;
    std::optional<sim::Packet> m_ingress_packet;
};
