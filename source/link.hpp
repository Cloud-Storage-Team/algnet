#pragma once

#include "packet.hpp"

namespace sim {

/**
 * Unidirectional link from the source to a_next
 */
class Link {
  public:
    Link(Node *a_next, int a_speed_mbps);

    /**
     * Update the queueing delay and schedule the Enqueue event
     * on the next node based on the queueing and transmission delays.
     */
    void process(Packet a_packet);

  private:
    Node *m_next;
    int m_speed_mbps;
    int m_queueing_delay;
    int m_transmission_delay;
};

}  // namespace sim
