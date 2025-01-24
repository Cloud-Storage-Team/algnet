#pragma once

#include "NetworkDevice.hpp"

#include <cstdint>
#include <memory>

class Link {
public:
    Link(std::shared_ptr<NetworkDevice> src, std::shared_ptr<NetworkDevice> dest, std::uint64_t distance_ns);

    void UpdateLastProcessTime(std::uint64_t new_processing_time_ns);

    std::shared_ptr<NetworkDevice> src;
    std::shared_ptr<NetworkDevice> dest;
    std::uint64_t delay_ns;

    // In bits per ns
    std::uint64_t bandwidth = 0;
    // Stores time when the last packet was pushed through
    std::uint64_t last_processing_time_ns = 0;
};
