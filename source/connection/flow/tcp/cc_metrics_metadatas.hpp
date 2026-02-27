#pragma once
#include "metrics/metrics_table/metrics_table.hpp"

namespace sim {

struct CCMetricMetadatas {
    static constexpr MetricId CWND = {"CWND", "Packets"};
};
}  // namespace sim
