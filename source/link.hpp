#pragma once

#include <queue>

namespace sim {

class Device;
class Packet;

/**
 * Unidirectional link from the source to a_next
 */
class Link {
public:
    Link(Device *a_dest, int a_speed_mbps);

    /**
     * Update the source egress delay and schedule the arrival event
     * based on the egress queueing and transmission delays.
     */
    void process(Packet a_packet);

private:
    Device *m_dest;
    int m_speed_mbps;
    int m_src_egress_delay;
    int m_transmission_delay;

    // Queue at the ingress port of the m_next device
    std::queue<Packet> m_next_ingress;
};

}  // namespace sim
