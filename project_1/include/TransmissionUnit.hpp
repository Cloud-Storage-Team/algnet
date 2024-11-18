#pragma once

#include <cstdint>

// Base struct for ACK, PacketHeader.
struct TransmissionUnit {
    explicit TransmissionUnit(std::uint64_t delivery_time_us);
    std::uint64_t GetEstimatedDeliveryTime() const;
protected:
    std::uint64_t estimated_delivery_time;
};
