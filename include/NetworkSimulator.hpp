#pragma once

#include "EventScheduler.hpp"
#include "Flow.hpp"
#include "Switch.hpp"
#include "Sender.hpp"
#include "Receiver.hpp"

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
    void StopAt(double time_ns);
    static void AddDevice(std::shared_ptr<NetworkDevice> device);
    static void EmplaceFlow(const std::vector<std::shared_ptr<NetworkDevice>>& path,
                            const std::vector<std::uint32_t>& distances_ns);
    static void Schedule(double delay, const std::function<void(void)>& handler);
    static double Now();

    static std::uint32_t GetDistanceNs(std::uint32_t src_id, std::uint32_t dst_id);
    static double GetLinkLastProcessTime(std::uint32_t src_id, std::uint32_t dst_id);
    static void UpdateLinkLastProcessTime(std::uint32_t src_id, std::uint32_t dst_id, double value);

    double stop_time_ns = 10000.0;
    static inline std::unique_ptr<EventScheduler> event_scheduler;
    static inline std::unordered_map<std::uint32_t, std::shared_ptr<NetworkDevice>> forward_routing_table;
    static inline std::unordered_map<std::uint32_t, std::shared_ptr<NetworkDevice>> backward_routing_table;
    static inline std::unordered_map<std::uint32_t, std::shared_ptr<NetworkDevice>> device_by_id;

    static inline double current_time_ns = 0.0;
    static const std::uint32_t packet_size_bytes = 1024;
    static const std::uint64_t bandwidth_bytes = 6'250'000'000;
    static inline std::vector<std::unique_ptr<Flow>> flows;
    static inline std::map<std::pair<std::uint32_t, std::uint32_t>, double> link_last_process_time_ns;
    static inline std::map<std::pair<std::uint32_t, std::uint32_t>, std::uint32_t> distances_ns;
};

namespace Time {
    double Seconds(double time_s);
    static const double s_to_ns = 1000000000.0;
}
