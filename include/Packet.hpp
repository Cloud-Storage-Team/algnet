#pragma once

#include <cstdint>

class Packet {
public:
    Packet(std::uint64_t sender_id, std::uint64_t sending_time_ns, std::uint64_t delivery_time_ns);
    Packet() = default;
    ~Packet() = default;

    std::uint64_t GetEstimatedDeliveryTime() const;
    std::uint32_t GetSenderId() const;

    bool operator<(const Packet& other) const;
private:
    std::uint32_t sender_id;
    std::uint64_t sending_time_ns;
    std::uint64_t estimated_delivery_time_ns;
};
