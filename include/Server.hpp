#pragma once

#include "NetworkDevice.hpp"
#include "Packet.hpp"

#include <vector>
#include <queue>

/**
 * @brief
 */
class Server: public NetworkDevice {
public:
    explicit Server(DeviceType type);
    ~Server() override = default;

    /**
     * @brief Sends events events to the event scheduler
     */
    void Send(std::uint32_t flow_id) override;

    /**
     * @brief Vector with packets sent by server
     */
    std::vector<std::shared_ptr<Packet>> sent_packets{};
};