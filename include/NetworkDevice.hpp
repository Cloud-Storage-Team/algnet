#pragma once

#include "Packet.hpp"

#include <vector>
#include <memory>
#include <cstdint>
#include <queue>

enum class DeviceType: std::uint8_t {
    Switch,
    Sender,
    Receiver,
};

class NetworkDevice {
public:
    explicit NetworkDevice(DeviceType type, double processing_delay_ns);
    virtual ~NetworkDevice() = default;

    virtual void ProcessPacket(Packet p) = 0;

    void Enqueue(Packet p);
    Packet Dequeue();
    bool Empty() const;
    std::shared_ptr<NetworkDevice> NextDevice() const;
    std::shared_ptr<NetworkDevice> PrevDevice() const;

    std::uint64_t id;
    std::queue<Packet> buffer;
    DeviceType type;
    double processing_delay_ns;
    double next_processing_time_ns = 0;
    inline static std::uint64_t last_given_device_id = 0;
};