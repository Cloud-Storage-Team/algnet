#pragma once

#include <cstdint>
#include <queue>

#include "packet.hpp"

namespace sim {

class RoutingModule;

/**
 * Unidirectional link from the source to a_next
 */
class Link {
public:
    Link() = default;
    virtual ~Link() = default;

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    virtual void schedule_arrival(Packet a_packet) = 0;

    virtual Packet get_packet() = 0;
    virtual std::shared_ptr<IRoutingDevice> get_from() const = 0;
    virtual std::shared_ptr<IRoutingDevice> get_to() const = 0;

//TODO: move to realisation
// private:
    // std::weak_ptr<IRoutingDevice> m_from;
    // std::weak_ptr<IRoutingDevice> m_to;
    // std::uint32_t m_speed_mbps;
    // std::uint32_t m_src_egress_delay;
    // std::uint32_t m_transmission_delay;

    // // Queue at the ingress port of the m_next device
    // std::queue<Packet> m_next_ingress;
};

}  // namespace sim
