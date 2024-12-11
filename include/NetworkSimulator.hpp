#pragma once

#include "Event.hpp"
#include "Flow.hpp"
#include "Switch.hpp"
#include "Server.hpp"

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
     * @brief Process events from the priority queue
     */
     void ProcessEvents();

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
    inline static std::vector<std::unique_ptr<Flow>> flows;

    /**
     * @brief Priority queue with all network events
     *
     * @details Events are sorted by delivery time
     */
    inline static std::priority_queue<Event> event_scheduler;

    /**
     * @brief Map: link (graph edge) --> time when link is ready to transmit next packet
     *
     * @details Key is a pair (min(a, b), max(a, b)), where a and b are IDs of devices connected by this link
     */
     inline static std::map<std::pair<std::uint32_t, std::uint32_t>, std::uint64_t> link_last_process_time_ns;

    /**
     * @brief Map: device ID --> index in `devices` vector
     */
    std::map<std::uint32_t, std::uint32_t> device_index_by_id;
};
