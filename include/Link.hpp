#pragma once

#include "NetworkDevice.hpp"

#include <cstdint>
#include <memory>

class Link {
public:
    Link(std::shared_ptr<NetworkDevice> source, std::shared_ptr<NetworkDevice> destination, std::uint64_t distance_ns);

    void UpdateLastProcessTime(std::uint64_t new_process_time_ns);

    std::shared_ptr<NetworkDevice> source;
    std::shared_ptr<NetworkDevice> destination;
    std::uint64_t distance_ns;

    // In bits per ns
    std::uint64_t speed = 0;
    // Stores time when the last packet was pushed through
    std::uint64_t last_process_time_ns = 0;
};