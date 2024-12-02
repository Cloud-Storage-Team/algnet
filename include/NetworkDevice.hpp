#pragma once

#include <vector>
#include <memory>
#include <cstdint>

enum class DeviceType: std::uint8_t {
    Switch,
    ComputerSenderNode,
    ComputerReceiverNode,
};

class NetworkDevice {
public:
    explicit NetworkDevice(DeviceType type);
    virtual ~NetworkDevice() = default;

    /**
     * @brief Vector of pointers to adjacent network devices
     *
     * Usage of std::weak_ptr is necessary to avoid cyclic links
     */
    std::vector<std::weak_ptr<NetworkDevice>> adjacent_devices;

    /**
     * @brief Network device type
     */
    DeviceType type;
};