#pragma once

#include "Packet.hpp"

#include <vector>
#include <memory>
#include <cstdint>
#include <queue>

class NetworkDevice {
public:
    explicit NetworkDevice(std::uint64_t processing_delay_ns);
    virtual ~NetworkDevice() = default;

    virtual void ProcessPacket(Packet p) = 0;

    void Enqueue(Packet p);
    Packet Dequeue();
    bool Empty() const;
    std::shared_ptr<NetworkDevice> NextDevice() const;
    std::shared_ptr<NetworkDevice> PrevDevice() const;

    std::uint64_t id;
    std::queue<Packet> buffer;
    std::uint64_t processing_delay_ns;
    std::uint64_t next_processing_time_ns = 0;
    inline static std::uint64_t last_given_device_id = 0;
};