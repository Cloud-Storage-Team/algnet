#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <queue>

#include "utils/identifier_factory.hpp"

namespace sim {

struct Packet;
class IRoutingDevice;

/**
 * Unidirectional link from the source to a_next
 */
class ILink : public Identifiable {
public:
    virtual ~ILink() = default;

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    virtual void schedule_arrival(Packet packet) = 0;

    /**
     * Removes packet from the source egress queue.
     */
    virtual void process_arrival(Packet packet) = 0;

    virtual std::optional<Packet> get_packet() = 0;
    virtual std::shared_ptr<IRoutingDevice> get_from() const = 0;
    virtual std::shared_ptr<IRoutingDevice> get_to() const = 0;
    virtual Size get_max_from_egress_buffer_size() const = 0;
};

class Link : public ILink, public std::enable_shared_from_this<Link> {
public:
    Link(Id a_id, std::weak_ptr<IRoutingDevice> a_from,
         std::weak_ptr<IRoutingDevice> a_to, std::uint32_t a_speed_gbps = 1,
         Time a_delay = 0, Size a_max_from_egress_buffer_size = 4096,
         Size a_max_to_ingress_buffer_size = 4096);
    ~Link() = default;

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    void schedule_arrival(Packet packet) final;

    /**
     * Removes packet from the source egress queue.
     */
    void process_arrival(Packet packet) final;

    std::optional<Packet> get_packet() final;

    std::shared_ptr<IRoutingDevice> get_from() const final;
    std::shared_ptr<IRoutingDevice> get_to() const final;
    Size get_max_from_egress_buffer_size() const final;

    Id get_id() const final;

private:
    Time get_transmission_time(const Packet& packet) const;

    Id m_id;
    std::weak_ptr<IRoutingDevice> m_from;
    std::weak_ptr<IRoutingDevice> m_to;
    std::uint32_t m_speed_gbps;

    Size m_from_egress_buffer_size;
    Size m_max_from_egress_buffer_size;
    Time m_arrival_time;

    Time m_transmission_delay;

    // Queue at the ingress port of the m_to device
    std::queue<Packet> m_to_ingress;
    // We keep track of m_to_ingress size in bytes
    // to account for packet size variations
    Size m_to_ingress_buffer_size;
    Size m_max_to_ingress_buffer_size;
};

}  // namespace sim
