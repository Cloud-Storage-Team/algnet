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
    Link(IRoutingDevice* a_from, IRoutingDevice* a_to,
         std::uint32_t a_speed_mbps, std::uint32_t m_delay);

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    void schedule_arrival(Packet a_packet);

private:
    IRoutingDevice* m_from;
    IRoutingDevice* m_to;
    std::uint32_t m_speed_mbps;
    std::uint32_t m_src_egress_delay;
    std::uint32_t m_transmission_delay;

    // Queue at the ingress port of the m_next device
    std::queue<Packet> m_next_ingress;
};

}  // namespace sim
