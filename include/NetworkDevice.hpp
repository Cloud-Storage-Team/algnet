#pragma once

#include "Packet.hpp"

#include <vector>
#include <memory>
#include <cstdint>
#include <unordered_map>

enum class DeviceType: std::uint8_t {
    Switch,
    ComputerSenderNode,
    ComputerReceiverNode,
};

class NetworkDevice {
public:
    explicit NetworkDevice(DeviceType type, std::uint32_t id);
    virtual ~NetworkDevice() = default;

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
     * @brief Network device ID.
     */
    std::uint32_t id;

    /**
     * @brief Network device storage.
     *
     * @details Device stores packets from adjacent device in corresponding vector. \n
     * Key: adjacent device ID. Value: vector with packets.
     */
    std::unordered_map<std::uint32_t, std::vector<Packet>> storage;
};