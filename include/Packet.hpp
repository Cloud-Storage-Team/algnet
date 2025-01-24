#pragma once

#include <cstdint>

class Packet {
public:
    explicit Packet(std::uint64_t src_id, std::uint64_t dest_id, std::uint64_t sending_time_ns, bool is_ack);
    Packet() = default;
    ~Packet() = default;

    std::uint64_t source_id;
    std::uint64_t destination_id;
    std::uint64_t sending_time_ns;
    std::uint32_t transmitted_data_size_bytes;
    bool is_ack;
};
