#pragma once

#include "packet.hpp"

#include <random>

class ExpressPass {
public:
    const static std::uint64_t min_packet_size = 84 * 8;
    const static std::uint64_t default_data_size = 1024 * 8;
    const static std::uint64_t target_link_speed = 50;
    const static std::uint64_t default_credit_rate_limit_ns = (default_data_size + target_link_speed - 1) / target_link_speed;   

    // TODO: replace 200 with correct value, corresponding to amount of flows (amount_of_flows / 10 or smth instead of 200)
    const static std::uint64_t default_inter_credit_gap_ns = 1 * (min_packet_size + default_data_size + target_link_speed - 1) / target_link_speed;
    const static std::uint64_t default_min_jitter = (default_inter_credit_gap_ns + 99) / 100;
    const static std::uint64_t default_max_jitter = default_min_jitter * 8;

    ExpressPass() = default;
    std::uint64_t getRandomJitter() const;
    PacketHeader GetCredit(std::uint64_t sending_time, std::uint64_t source, std::uint64_t destination,  std::uint32_t index = 0, std::uint64_t size = min_packet_size) const;
    PacketHeader GetHandshakePacket(std::uint64_t sending_time, std::uint64_t source, std::uint64_t destination, std::uint64_t size = min_packet_size) const;
    PacketHeader GetDataPacket(std::uint64_t sending_time, std::uint64_t source, std::uint64_t destination, std::uint32_t index = 0, std::uint64_t rtt = 0, std::uint64_t size = default_data_size) const;
};