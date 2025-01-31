#pragma once

#include <queue>
#include <unordered_map>

#include "flow.hpp"
#include "link.hpp"
#include "packet.hpp"

namespace sim {

class Node {
  public:
    /**
     * Update the queueing delay, insert a packet into the ingress
     * buffer and schedule the Dequeue event based on the queueing delay.
     */
    void enqueue(Packet a_packet);

    // Dequeue a packet from the ingress buffer
    void dequeue();

    // Create a link to the next node
    void link(Node* a_next, int m_speed_mbps);

    // BFS to update the routing table
    void recalculate_paths();

  private:
    int m_speed_mbps;
    int m_queueing_delay;
    std::vector<Flow> m_flows;
    std::queue<Packet> m_buffer;
    // A routing table: it maps distination node to a specific link
    std::unordered_map<Node*, Link> m_links;
};

}  // namespace sim
