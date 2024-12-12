#pragma once

#include "ACK.hpp"
#include "CongestionWindowHandler.hpp"
#include "PacketHeader.hpp"
#include "ServerBase.hpp"

#include "memory"

// Type for sending server, contains ID and distance to receiver
struct ServerSender : ServerBase {
    ServerSender(std::uint64_t id, std::uint32_t distance_us);
    ServerSender() = default;
    ServerSender(ServerSender &&other) = default;
    virtual ~ServerSender() = default;

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
    void SendPackets(std::priority_queue<PacketHeader>& packet_queue);
private:
    const std::uint32_t max_transmission_rate_packets = 6'250'000;
    const std::uint32_t distance_us;
    std::uint32_t current_time_us = 0;
    std::uint32_t cwnd_size_in_packets = 1;

    std::vector<ACK> ACKs{};

    std::unique_ptr<CongestionWindowHandler> cwnd_handler;
};