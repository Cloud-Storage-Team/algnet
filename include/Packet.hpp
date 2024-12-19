#pragma once

#include <cstdint>

class Packet {
public:
    explicit Packet(std::uint64_t src_id, std::uint64_t dst_id, std::uint64_t sending_time_ns, bool is_ack);
    Packet() = default;
    ~Packet() = default;

    std::uint64_t m_source_id;
    std::uint64_t m_destination_id;
    std::uint64_t m_sending_time_ns;
    std::uint32_t m_transmitted_data_size_bytes;
    bool m_is_ack;
};
