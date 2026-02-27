#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "metrics/metrics_storage.hpp"

namespace sim {

struct MetricId {
    std::string name;       // e.g. RTT/throughput
    std::string unit_name;  // e.g. ns,Mbps

    auto operator<=>(const MetricId&) const = default;
};

}  // namespace sim

namespace std {
template <>
struct hash<sim::MetricId> {
    static constexpr std::size_t RANDOM_VALUE = 0x23478;
    static constexpr std::size_t RANDOM_SHIFT_1 = 6;
    static constexpr std::size_t RANDOM_SHIFT_2 = 2;

    std::size_t operator()(const sim::MetricId& metric_id) const noexcept {
        size_t h1 = hash<string>{}(metric_id.name);
        size_t h2 = hash<string>{}(metric_id.unit_name);
        return h1 ^ (h2 + RANDOM_VALUE + (h1 << RANDOM_SHIFT_1) +
                     (h1 >> RANDOM_SHIFT_2));
    }
};

}  // namespace std

namespace sim {

class MetricsTable
    : public std::unordered_map<MetricId,
                                std::shared_ptr<const MetricsStorage>> {
public:
    using std::unordered_map<
        MetricId, std::shared_ptr<const MetricsStorage>>::unordered_map;

    void draw_pictures(std::filesystem::path output_dir) const;
};

}  // namespace sim
