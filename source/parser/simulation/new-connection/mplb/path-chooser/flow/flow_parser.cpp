#include "flow_parser.hpp"

#include "connection/flow/tcp/new_tcp_flow.hpp"
#include "parser/parse_utils.hpp"

namespace sim {
std::shared_ptr<NewTcpFlow> parse_tcp_flow(
    const ConfigNodeWithPreset& flow_node, Endpoints endpoints);

std::shared_ptr<INewFlow> parse_i_flow(const ConfigNodeWithPreset& flow_node,
                                       Endpoints endpoints) {
    std::string type =
        flow_node["type"].value_or_throw().as_or_throw<std::string>();
    if (type == "tcp") {
        return parse_tcp_flow(flow_node, std::move(endpoints));
    }
    throw flow_node.create_parsing_error(
        fmt::format("Unsupported flow type: {}", type));
}

RTO parse_rto(const ConfigNodeWithPreset& node) {
    return RTO(parse_time(node["start"].value().get_node()),
               parse_time(node["max"].value().get_node()));
}

TcpFlowMetricsFilters parse_metrics_flags(const ConfigNodeWithPreset& node) {
    TcpFlowMetricsFilters flags;
    if (const ConfigNodeWithPresetExpected& rtt_node = node["rtt"]; rtt_node) {
        flags.rtt = rtt_node.value().as_or_throw<bool>();
    }
    if (const ConfigNodeWithPresetExpected& delivery_rate_node =
            node["delivery_rate"];
        delivery_rate_node) {
        flags.delivery_rate = delivery_rate_node.value().as_or_throw<bool>();
    }
    if (const ConfigNodeWithPresetExpected& reordering_node =
            node["reordering"];
        reordering_node) {
        flags.reordering = reordering_node.value().as_or_throw<bool>();
    }
    return flags;
}

std::shared_ptr<NewTcpFlow> parse_tcp_flow(
    const ConfigNodeWithPreset& flow_node, Endpoints endpoints) {
    const ConfigNodeWithPresetExpected& rto_node = flow_node["rto"];
    RTO rto = (rto_node ? parse_rto(rto_node.value())
                        : NewTcpFlow::DEFAULT_START_RTO);

    bool ecn_capable = flow_node["ecn_capable"].as<bool>().value_or(
        NewTcpFlow::DEFAULT_ECN_CAPABLE);

    const ConfigNodeWithPresetExpected& metrics_flags_node =
        flow_node["metrics_filters"];
    TcpFlowMetricsFilters metrics_flags =
        (metrics_flags_node ? parse_metrics_flags(metrics_flags_node.value())
                            : NewTcpFlow::DEFAULT_METRICS_FLAGS);

    return NewTcpFlow::create_shared(flow_node.get_name_or_throw(),
                                     endpoints.sender, endpoints.receiver,
                                     ecn_capable, rto, metrics_flags);
}

}  // namespace sim
