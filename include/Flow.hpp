#pragma once

#include "NetworkDevice.hpp"
#include "Event.hpp"

#include <cstdint>
#include <memory>
#include <queue>

/**
 * @brief Network flow between the source and destination nodes
 */
class Flow {
public:
    explicit Flow(const std::vector<std::shared_ptr<NetworkDevice>>& path);

    /**
     * @brief Send packets from every server sender in the amount of CWND size.
     */
    void Send();

    /**
     * @brief Vector of network devices forming the flow path
     *
     * @details First element is the source node, last element is the destination node
     */
    std::vector<std::shared_ptr<NetworkDevice>> path;

    /**
     * @brief Vector of distances in nanoseconds between nodes on the path
     */
    std::vector<std::uint32_t> distances_ns;

};
