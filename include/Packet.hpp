#pragma once

#include <cstdint>

class Packet {
public:
    explicit Packet(std::uint64_t sending_time_ns);
    Packet() = default;
    ~Packet() = default;

    /**
     * @brief Packet sending time in nanoseconds
     */
    std::uint64_t sending_time_ns;

    /**
     * @brief Number of successfully delivered bytes
     */
    std::uint32_t transmitted_data_size_bytes;
};
