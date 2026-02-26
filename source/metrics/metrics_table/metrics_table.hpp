#pragma once
#include <map>
#include <string>

#include "metrics/metrics_storage.hpp"

namespace sim {

struct MetricId {
    std::string name;
    std::string unit_name;

    auto operator<=>(const MetricId&) const = default;
};

using MetricsTable = std::map<MetricId, const MetricsStorage&>;

}  // namespace sim
