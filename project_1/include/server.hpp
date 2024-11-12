#pragma once

#include "packet.hpp"

#include <cstdint>
#include <vector>
#include <queue>

// Type for server receiver, contains only ID
struct ServerBase {
protected:
    std::uint64_t id;
public:
    ServerBase(std::uint32_t id);
    ServerBase() = default;

    std::uint32_t GetID() const;
};

// Type for sending server, contains ID and distance to receiver
struct ServerSender : ServerBase {
private:
    std::uint32_t distance_us;
    std::uint32_t current_time_us = 0;
    std::uint32_t cwnd_size_in_packets = 1;
    std::uint32_t max_transmission_rate_packets = 6'250'000;

    std::priority_queue<PacketHeader>& packet_queue;
    std::vector<ACK> ACKs{};
public:
    ServerSender(std::uint32_t id, std::uint32_t distance_us, std::priority_queue<PacketHeader>& queue);
    ServerSender() = default;

    std::uint32_t GetDistance() const;
    std::uint32_t GetACKsNumber() const;
    std::uint32_t GetCWNDSize() const;
    std::uint32_t GetCurrentTime() const;
    void UpdateCurrentTime(std::uint32_t new_time_us);
    void IncreaseCurrentTime(std::uint32_t delta_time_us);
    void IncreaseCWNDSize();

    void AddACK(const ACK& ack);
    bool HandleACKs();
    void printACKs() const;

    void SendPackets();
};