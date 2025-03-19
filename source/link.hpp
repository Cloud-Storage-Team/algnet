<<<<<<< HEAD
#pragma once

#include <cstdint>
#include <queue>

#include "packet.hpp"

namespace sim {

class Device;

/**
 * Unidirectional link from the source to a_next
 */
class Link {
public:
    Link(Device* a_from, Device* a_to, std::uint32_t a_speed_mbps,
         std::uint32_t m_delay);

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    void schedule_arrival(Packet a_packet){
        // TODO: implement and move to .cpp
    };

    // TODO: move to .cpp
    Packet get_packet() {
        Packet result = m_next_ingress.front();
        m_next_ingress.pop();
        return result;
    };

private:
    Device* m_from;
    Device* m_to;
    std::uint32_t m_speed_mbps;
    std::uint32_t m_src_egress_delay;
    std::uint32_t m_transmission_delay;

    // Queue at the ingress port of the m_next device
    std::queue<Packet> m_next_ingress;
};

}  // namespace sim
=======
#pragma once

#include <cstdint>
#include <optional>
#include <queue>

#include "packet.hpp"

namespace sim {

class RoutingModule;

/**
 * Unidirectional link from the source to a_next
 */
class ILink {
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
};

class Link : ILink {
public:
    Link(std::weak_ptr<IRoutingDevice> a_from,
         std::weak_ptr<IRoutingDevice> a_to, std::uint32_t a_speed_mbps = 1,
         std::uint32_t a_delay = 0);
    virtual ~Link() = default;

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    virtual void schedule_arrival(Packet packet) final;

    /**
     * Removes packet from the source egress queue.
     */
    virtual void process_arrival(Packet packet) final;

    virtual std::optional<Packet> get_packet() final;
    virtual std::shared_ptr<IRoutingDevice> get_from() const final;
    virtual std::shared_ptr<IRoutingDevice> get_to() const final;

private:
    std::uint32_t get_transmission_time(const Packet& packet) const;

    std::weak_ptr<IRoutingDevice> m_from;
    std::weak_ptr<IRoutingDevice> m_to;
    std::uint32_t m_speed_mbps;
    std::uint32_t m_src_egress_delay;
    std::uint32_t m_transmission_delay;

    // Queue at the ingress port of the m_next device
    std::queue<Packet> m_next_ingress;
};

}  // namespace sim
>>>>>>> origin/main
