#pragma once

#include <random>

#include "packet.hpp"

class PacketGenerator {
  public:
    const static std::uint64_t min_packet_size = 84 * 8;
    const static std::uint64_t default_data_size = 1024 * 8;

    PacketGenerator() = default;
    PacketHeader GetAckPacket(std::uint64_t sending_time, std::uint64_t source,
                              std::uint64_t destination,
                              std::uint64_t size = min_packet_size) const;
    PacketHeader GetDataPacket(std::uint64_t sending_time, std::uint64_t source,
                               std::uint64_t destination,
                               std::uint64_t size = default_data_size) const;
};