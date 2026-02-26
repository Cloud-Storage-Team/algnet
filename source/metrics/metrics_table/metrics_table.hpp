#pragma once
#include <spdlog/fmt/fmt.h>

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
    std::size_t operator()(const sim::MetricId& metric_id) const noexcept {
        return std::hash<std::string>()(metric_id.name) ^
               std::hash<std::string>()(metric_id.unit_name);
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
