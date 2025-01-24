#pragma once

#include "EventScheduler.hpp"
#include "Flow.hpp"
#include "Switch.hpp"
#include "Sender.hpp"
#include "Receiver.hpp"
#include "Link.hpp"

#include <cstdint>
#include <vector>
#include <memory>
#include <queue>
#include <unordered_map>
#include <map>

class NetworkSimulator {
public:
    NetworkSimulator() = default;
    ~NetworkSimulator() = default;

    static inline bool EnableACK = true;

    void Run() const;
    void StopAt(std::uint64_t time_ns);
    static void AddDevice(std::shared_ptr<NetworkDevice> device);
    static void AddLink(std::shared_ptr<Link> link, std::shared_ptr<Flow> flow);
    static void AddFlow(std::shared_ptr<Flow> flow);
    static void Schedule(std::uint64_t delay, const std::function<void(void)>& handler);
    static std::uint64_t Now();

    std::uint64_t stop_time_ns = 1'000'000'000;
    static inline std::unique_ptr<EventScheduler> event_scheduler;
    static inline std::map<std::pair<std::uint32_t, std::uint32_t>, std::shared_ptr<Link>> routing_table;
    static inline std::unordered_map<std::uint32_t, std::shared_ptr<NetworkDevice>> device_by_id;

    static inline std::uint64_t current_time_ns = 0;
    static const std::uint32_t packet_size_bytes = 1024;
    static const std::uint64_t bandwidth_bytes = 6'250'000'000;
    static inline std::vector<std::shared_ptr<Flow>> flows;

    static void AddNewFlow(std::shared_ptr<NetworkDevice> sender, std::shared_ptr<NetworkDevice> receiver, std::vector<std::shared_ptr<Switch>>& switches);

    static void GenerateTree(std::shared_ptr<NetworkDevice> receiver, int maxDepth, int k, std::vector<std::shared_ptr<Switch>>& currentSwitches, int currentDepth = 0);
};

namespace Time {
    std::uint64_t Seconds(std::uint64_t time_s);
    std::uint64_t Milliseconds(std::uint64_t time_ms);
    static const std::uint64_t s_to_ns = 1'000'000'000;
    static const std::uint64_t ms_to_ns = 1'000'000;
}
