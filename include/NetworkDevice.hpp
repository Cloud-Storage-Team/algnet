#pragma once

#include "Packet.hpp"

#include <vector>
#include <memory>
#include <cstdint>
#include <queue>
#include <unordered_map>

/* Forward declaration of Link class is necessary
 because NetworkDevice uses Link and Link uses NetworkDevice */
class Link;

class NetworkDevice {
public:
    explicit NetworkDevice(std::uint64_t processing_delay_ns);
    virtual ~NetworkDevice() = default;

    virtual void ProcessPacket(Packet p) = 0;

    void Enqueue(Packet p);
    Packet Dequeue();
    bool Empty() const;
    std::shared_ptr<Link> NextLink(std::uint32_t destination_id) const;

    std::uint64_t id;
    std::queue<Packet> buffer;
    std::unordered_map<std::uint32_t, std::shared_ptr<Link>> routing_table;
    std::uint64_t processing_delay_per_packet;
    std::uint64_t completion_time = 0;
    inline static std::uint64_t last_given_device_id = 0;
};