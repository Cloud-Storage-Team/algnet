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
#include <map>

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
     * @brief Network graph topological sorting
     */
    void Topsort();

    /**
     * @brief Size of a packet in bytes.
     */
    static const std::uint32_t packet_size_bytes = 1024;

    /**
     * @brief Size of a receiver's bandwidth in bytes.
     */
    static const std::uint64_t bandwidth_bytes = 6'250'000'000;

    /**
     * @brief Number of simulation iterations
     */
     std::uint32_t iteration_num = 10;

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

    /**
     * @brief Topsorted forward traversal of a network graph
     *
     * @details Forward means from senders to receivers (Packets transmission)
     */
    std::vector<std::uint32_t> forward_device_visit_order;

    /**
     * @brief Topsorted backward traversal of a network graph
     *
     * @details Backward means from receivers to senders (ACKs transmission)
     */
    std::vector<std::uint32_t> backward_device_visit_order;

    /**
     * @brief Map: device ID --> index in `devices` vector
     */
    std::map<std::uint32_t, std::uint32_t> device_by_id;
};
