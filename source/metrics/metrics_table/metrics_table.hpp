#pragma once
#include <map>
#include <string>

#include "metrics/metrics_storage.hpp"

namespace sim {

struct MetricId {
    std::string name;       // e.g. RTT/throughput
    std::string unit_name;  // e.g. ns,Mbps

    auto operator<=>(const MetricId&) const = default;
};

using MetricsTable = std::map<MetricId, const MetricsStorage&>;

}  // namespace sim
