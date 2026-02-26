#pragma once
#include <string>
#include <unordered_map>

#include "metrics/metrics_storage.hpp"

namespace sim {

struct MetricMetadata {
    std::string name;       // e.g. RTT/throughput
    std::string unit_name;  // e.g. ns,Mbps

    auto operator<=>(const MetricMetadata&) const = default;
};

using MetricsTable = std::unordered_map<MetricMetadata, const MetricsStorage&>;

}  // namespace sim
