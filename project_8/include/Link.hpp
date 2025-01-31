#pragma once

#include "NetworkDevice.hpp"

#include <cstdint>
#include <memory>

class Link {
public:
    Link(std::shared_ptr<NetworkDevice> first_device, std::shared_ptr<NetworkDevice> second_device, std::uint64_t distance_ns);

    void UpdateLastProcessTime(std::uint64_t new_process_time_ns);
    
    std::uint64_t GetDistance();
    std::uint64_t GetLastProcessTime();
    
    std::shared_ptr<NetworkDevice> GetNextDevice(uint32_t id);
    private:
    std::shared_ptr<NetworkDevice> first_device_;
    std::shared_ptr<NetworkDevice> second_device_;
    std::uint64_t distance_;

    // In bits per ns
    std::uint64_t transition_speed_ = 0;
    std::uint64_t last_process_time_ns_ = 0;
};