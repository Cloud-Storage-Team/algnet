#include "metricable_cc_parser.hpp"

#include "parser/simulation/connection/flow/tcp/cc/tcp_cc_parser.hpp"

namespace sim {
MetricableCC parse_metricable_cc(const ConfigNodeWithPreset& node) {
    // TODO: replace ConfigNode with ConfigNodeWithPreset inside CC parser
    std::unique_ptr<ITcpCC> cc = TcpCCParser::parse_i_tcp_cc(node.get_node());

    MetricableCcMetricsFilters flags = MetricableCC::DEFAULT_FLAGS;
    if (const auto& flags_node = node["metrics_filters"]; flags_node) {
        if (const auto& cwnd_node = flags_node["cwnd"]; cwnd_node) {
            flags.cwnd = cwnd_node.value().as_or_throw<bool>();
        }
    }

    return MetricableCC(std::move(cc), std::move(flags));
}
}  // namespace sim