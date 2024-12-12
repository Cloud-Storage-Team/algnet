#pragma once

#include "INTHeader.hpp"

#include <cstdint>
#include <vector>

// Base struct for ACK, PacketHeader.
struct TransmissionUnit {
    explicit TransmissionUnit(std::uint64_t delivery_time_us);
    std::uint64_t GetEstimatedDeliveryTime() const;

    std::uint64_t estimated_delivery_time;
    std::vector<INTHeader> headers;
};
