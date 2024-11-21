#pragma once

#include "ServerBase.hpp"
#include "ACK.hpp"
#include "PacketHeader.hpp"

// Type for sending server, contains ID and distance to receiver
struct ServerSender : ServerBase {
    ServerSender(std::uint64_t id, std::uint32_t data_rate_gbps);
    ServerSender() = default;
    virtual ~ServerSender() = default;

    std::uint32_t GetDataRate() const;
    std::uint32_t GetPacketTransmissionTime() const;
    std::uint32_t GetACKsNumber() const;
    std::uint32_t GetCWNDSize() const;
    std::uint32_t GetCurrentTime() const;

    void UpdateCurrentTime(std::uint64_t new_time_ns);
    void IncreaseCurrentTime(std::uint64_t delta_time_ns);
    void IncreaseCWNDSize();
    void AddACK(const ACK& ack);
    bool HandleACKs();
    void printACKs() const;
    void SendPackets(std::priority_queue<PacketHeader>& packet_queue);
private:
    const std::uint32_t max_data_rate_gbps;
    std::uint32_t max_data_rate_packets;
    std::uint64_t current_time_ns = 0;
    std::uint32_t cwnd_size_in_packets = 1;
    std::uint32_t packet_transmission_time;

    std::vector<ACK> ACKs{};
};