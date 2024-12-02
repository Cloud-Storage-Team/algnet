#pragma once

#include <cstdint>

class Packet {
public:
    Packet(std::uint64_t sender_id, std::uint64_t sending_time_ns, std::uint64_t delivery_time_ns);
    Packet() = default;
    ~Packet() = default;

    bool operator<(const Packet& other) const;

    /**
     * @brief ID of the sending network device
     */
    std::uint32_t sender_id;

    /**
     * @brief Packet sending time in nanoseconds
     */
    std::uint64_t sending_time_ns;

    /**
     * @brief Packet delivery time in nanoseconds
     */
    std::uint64_t estimated_delivery_time_ns;
};
