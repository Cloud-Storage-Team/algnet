#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <queue>

enum class DeviceType: std::uint8_t {
    Switch,
    ServerSender,
    ServerReceiver,
};

class NetworkDevice {
public:
    explicit NetworkDevice(DeviceType type);
    virtual ~NetworkDevice() = default;

    /**
     * @brief Send events (i. e. packets) to the receiver node
     *
     * @details Receiver -- next device in the flow path
     */
    virtual void Send(std::uint32_t flow_id) = 0;

    /**
     * @brief Vector of pointers to adjacent network devices.
     *
     * @details Usage of std::weak_ptr is necessary to avoid cyclic links.
     */
    std::vector<std::weak_ptr<NetworkDevice>> adjacent_devices;

    /**
     * @brief Network device type.
     */
    DeviceType type;

    /**
     * @brief Device's current time in nanoseconds
     */
    std::uint64_t current_time_ns = 0;

    /**
     * @brief Device ID
     *
     * Usage: ID is a vertex in the network graph
     */
    std::uint32_t id;

    /**
     * @brief New device ID = last given + 1
     */
    inline static std::uint32_t last_given_device_id = 0;
};