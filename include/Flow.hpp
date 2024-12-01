#pragma once

#include "Packet.hpp"
#include "Server.hpp"

#include <cstdint>
#include <vector>
#include <queue>


/**
 * Packet flow between source and destination nodes (servers).
 */
class Flow {
public:
    explicit Flow(const std::vector<std::uint32_t>& distances);
    Flow() = default;
    ~Flow() = default;

   /**
    * Send packets from every server sender in the amount of CWND size.
    */
    void Send();

   /**
    * Get size of priority queue in packets.
    *
    * @return std::uint32_t -- size of a priority queue in packets.
    */
    std::uint32_t GetPriorityQueueSize() const;

   /**
    * Get vector with source nodes (senders).
    *
    * @return const std::vector<ServerSender>& -- reference to a vector with source nodes.
    */
    const std::vector<ServerSender>& GetSourceNodes() const;

    /**
    * Get destination node (receiver).
    *
    * @return const ServerBase& -- reference to a destination node.
    */
    const ServerBase& GetDestinationNode() const;
private:
    std::uint32_t last_given_server_id = 0;

    std::vector<ServerSender> source_nodes;
    ServerBase destination_node;

    std::priority_queue<Packet> packets;
};
