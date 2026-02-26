#pragma once
#include "metrics/metrics_table/metrics_table.hpp"

namespace sim {

struct FlowMetricMetadatas {
    static constexpr MetricId RTT = {"RTT", "ns"};
    static constexpr MetricId DELIVERY_RATE = {"Delivery rate", "Mpbs"};
    static constexpr MetricId PACKET_REORDERING = {"Reordering", "Invs count"};
};
}  // namespace sim
