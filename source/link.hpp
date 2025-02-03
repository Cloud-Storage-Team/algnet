#pragma once

#include <queue>

#include "packet.hpp"

namespace sim {

/**
 * Unidirectional link from the source to a_next
 */
class Link {
public:
    Link(Device *a_next, int a_speed_mbps);

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    void process(Packet a_packet);

private:
    Device *m_next;
    int m_speed_mbps;
    int m_src_egress_delay;
    int m_transmission_delay;

    // Queue at the ingress port of the m_next device
    std::queue<Packet> m_next_ingress;
};

}  // namespace sim
