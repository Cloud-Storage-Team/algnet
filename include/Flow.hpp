#pragma once

#include "NetworkDevice.hpp"

#include <cstdint>
#include <memory>
#include <queue>
#include <utility>

/**
 * @brief Network flow between the source and destination nodes
 */
class Flow {
public:
    explicit Flow(const std::vector<std::shared_ptr<NetworkDevice>>& path, const std::vector<std::uint32_t>& distances_ns);

    /**
     * @brief Send packets from server sender.
     */
    void Send();

    /**
     * @brief Find next device on the path
     *
     * @param sender_id -- ID of sending device
     *
     * @returns {ID, distance > 0} -- next device found
     * @returns {0, 0} -- next device not found
     */
    std::pair<std::uint32_t, std::uint32_t> FindNextDeviceInPath(std::uint32_t sender_id);

    /**
     * @brief ID of the flow
     *
     * Usage: event routing in the network
     */
    std::uint32_t id;

    /**
     * @brief Vector of network devices forming the flow path
     *
     * @details First element is the source node, last element is the destination node
     */
    std::vector<std::shared_ptr<NetworkDevice>> path{};

    /**
     * @brief Vector of distances in nanoseconds between nodes on the path
     */
    std::vector<std::uint32_t> distances_ns{};

    /**
     * @brief New flow ID = last given + 1
     */
    inline static std::uint32_t last_given_flow_id = 0;

};
