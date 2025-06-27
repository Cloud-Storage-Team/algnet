#pragma once

#include <vector>

#include "link/interfaces/i_link.hpp"
#include "packet.hpp"

class LinkMock : public sim::ILink {
public:
    LinkMock(std::weak_ptr<sim::IRoutingDevice> a_from,
             std::weak_ptr<sim::IRoutingDevice> a_to);
    ~LinkMock() = default;
    virtual void schedule_arrival(sim::Packet a_packet) final;
    virtual void process_arrival(sim::Packet packet) final;
    virtual std::optional<sim::Packet> get_packet() final;
    virtual std::shared_ptr<sim::IRoutingDevice> get_from() const final;
    virtual std::shared_ptr<sim::IRoutingDevice> get_to() const final;
    
    virtual Size get_from_egress_queue_size() const final;
    virtual Size get_max_from_egress_buffer_size() const final;

    virtual Size get_to_ingress_queue_size() const final;
    virtual Size get_max_to_ingress_queue_size() const final;

    void set_ingress_packet(sim::Packet a_paket);
    std::vector<sim::Packet> get_arrived_packets() const;

    Id get_id() const final;

private:
    std::weak_ptr<sim::IRoutingDevice> m_from;
    std::weak_ptr<sim::IRoutingDevice> m_to;
    std::vector<sim::Packet> m_arrived_packets;
    std::optional<sim::Packet> m_ingress_packet;
};
