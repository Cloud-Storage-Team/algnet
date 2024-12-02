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
     * @brief Send packets from every server sender in the amount of CWND size.
     */
    void Send();

    /**
    * @brief Last ID given to network device.
    */
    std::uint32_t last_given_id = 0;

    /**
     * @brief Vector with sending server objects
     */
    std::vector<ServerSender> source_nodes;

    /**
     * @brief Server receiver object
     */
    ServerBase destination_node;

    /**
     * @brief Priority queue with in-flight packets
     */
    std::priority_queue<Packet> packets;
};