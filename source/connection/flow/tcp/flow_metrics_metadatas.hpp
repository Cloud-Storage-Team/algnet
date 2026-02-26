#pragma once
#include "metrics/metrics_table/metrics_table.hpp"
#include "units/units.hpp"

namespace sim {

struct FlowMetricMetadatas {
    static constexpr MetricMetadata RTT = MetricMetadata{"RTT", "ns"};
    static constexpr MetricMetadata DELIVERY_RATE =
        MetricMetadata{"Delivery rate", "Gbps"};
    static constexpr MetricMetadata PACKET_REORDERING =
        MetricMetadata{"Reordering", "Invs count"};
};

}  // namespace sim
