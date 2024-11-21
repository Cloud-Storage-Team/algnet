#pragma once

#include "TransmissionUnit.hpp"

// Type for packet
struct PacketHeader : TransmissionUnit {
    PacketHeader(std::uint64_t sender_id, std::uint64_t sending_time_ns, std::uint64_t delivery_time_ns);
    PacketHeader() = default;
    std::uint64_t GetSendingTime() const;
    std::uint64_t GetSenderID() const;

    bool operator<(const PacketHeader& other) const;
private:
    std::uint64_t sender_id;
    std::uint64_t sending_time;
};