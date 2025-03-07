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
    // TODO: move implementation to .cpp
    Link(std::shared_ptr<IRoutingDevice> a_from, std::shared_ptr<IRoutingDevice> a_to, std::uint32_t a_speed_mbps,
         std::uint32_t a_delay)
        : m_from(a_from),
          m_to(a_to),
          m_speed_mbps(a_speed_mbps),
          m_src_egress_delay(a_delay),
          m_transmission_delay(0),
          m_next_ingress(std::queue<Packet>()) {}
    virtual ~Link() = default;

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    void schedule_arrival(Packet a_packet);
    // TODO: move implementation to .cpp
    std::shared_ptr<IRoutingDevice> get_dest() { return m_to; };

private:
    std::shared_ptr<IRoutingDevice> m_from;
    std::shared_ptr<IRoutingDevice> m_to;
    std::uint32_t m_speed_mbps;
    std::uint32_t m_src_egress_delay;
    std::uint32_t m_transmission_delay;

    // Queue at the ingress port of the m_next device
    std::queue<Packet> m_next_ingress;
};

}  // namespace sim
