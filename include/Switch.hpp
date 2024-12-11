#pragma once

#include "NetworkDevice.hpp"

#include <unordered_map>

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
     * @brief Map: flow_id --> next device in the path
     */
    std::unordered_map<std::uint32_t, std::pair<std::uint32_t, std::uint32_t>> switching_table;
};