#pragma once

#include "NetworkDevice.hpp"

#include <unordered_map>
#include <tuple>

/**
 * @brief
 */
class Switch: public NetworkDevice {
public:
    Switch();
    ~Switch() override = default;

    /**
     * @brief Sends events (i. e. packets) to the receiver
     *
     * @details Receiver -- next device in the flow path
     */
    void Send(std::uint32_t flow_id) override;

    /**
     * @brief Map: flow_id --> adjacent devices on the flow path
     *
     * @details see FindAdjDevicesInPath(std::uint32_t) in Flow.hpp
     */
    std::unordered_map<std::uint32_t, std::tuple<std::uint32_t, std::uint64_t, std::uint32_t, std::uint64_t>> switching_table;
};