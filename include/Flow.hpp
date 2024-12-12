#pragma once

#include "NetworkDevice.hpp"

#include <cstdint>
#include <memory>
#include <queue>
#include <tuple>

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
     * @brief Find adjacent devices on the path
     *
     * @param id -- ID of current device
     *
     * @returns tuple {PrevID, PrevDist, NextID, NextDist}
     * @returns PrevID -- ID of previous (closer to sender) device on the path
     * @returns PrevDist -- distance in nanoseconds to device with ID = PrevID
     * @returns NextID -- ID of next (closer to receiver) device on the path
     * @returns NextDist -- distance in nanoseconds to device with ID = NextID
     */
    std::tuple<std::uint32_t, std::uint64_t, std::uint32_t, std::uint64_t> FindAdjDevicesInPath(std::uint32_t id);

    /**
     * @brief Names for FindAdjDevicesInPath(id) return values
     */
    static const std::uint8_t PrevID = 0;
    static const std::uint8_t PrevDist = 1;
    static const std::uint8_t NextID = 2;
    static const std::uint8_t NextDist = 3;

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
