#pragma once

#include "Flow.hpp"
#include "NetworkDevice.hpp"
#include "Switch.hpp"
#include "Server.hpp"
#include "Event.hpp"

#include <cstdint>
#include <vector>
#include <memory>
#include <queue>

/**
 * @brief Implementation of a network simulator.
 */
class NetworkSimulator {
public:
    NetworkSimulator() = default;
    ~NetworkSimulator() = default;

    /**
     * @brief Start simulation method.
     */
    void Run();

    /**
     * @brief Size of a packet in bytes.
     */
    static const std::uint32_t packet_size_bytes = 1024;

    /**
     * @brief Size of a receiver's bandwidth in bytes.
     */
    static const std::uint64_t bandwidth_bytes = 6'250'000'000;

    /**
     * @brief Vector with all network devices
     */
    std::vector<std::shared_ptr<NetworkDevice>> devices;

    /**
     * @brief Vector with all network flows
     *
     * @details Flow -- is a path from sender to receiver in the network
     */
    std::vector<std::unique_ptr<Flow>> flows;
};
