#pragma once

#include "NetworkDevice.hpp"

#include <cstdint>
#include <memory>
#include <queue>

class Flow {
public:
    explicit Flow(const std::vector<std::shared_ptr<NetworkDevice>>& path, const std::vector<std::uint32_t>& distances_ns);

    void Send();

    std::uint32_t id;
    std::vector<std::shared_ptr<NetworkDevice>> path{};
    std::vector<std::uint32_t> distances_ns{};

    double interval_ns;
    inline static std::uint32_t last_given_flow_id = 0;
};
