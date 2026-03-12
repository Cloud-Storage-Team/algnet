#include "path_chooser_parser.hpp"

#include "network/connection/mplb/path_chooser/round_robin/round_robin_path_chooser.hpp"
#include "parser/network/new-connection/mplb/path-chooser/flow/flow_parser.hpp"

namespace sim {

std::unique_ptr<RoundRobinPathChooser> parse_round_robin_path_chooser(
    ConfigNodeWithPreset node, Endpoints endpoints);

std::unique_ptr<IPathChooser> parse_i_path_chooser(
    const ConfigNodeWithPreset& node, Endpoints endpoints) {
    std::string type = node["type"].value_or_throw().as_or_throw<std::string>();
    if (type == "round-robin") {
        return parse_round_robin_path_chooser(node, std::move(endpoints));
    }
    throw node.create_parsing_error(
        fmt::format("Unsupported path chooser type: {}", type));
}

std::unique_ptr<RoundRobinPathChooser> parse_round_robin_path_chooser(
    ConfigNodeWithPreset node, Endpoints endpoints) {
    ConfigNodeWithPreset flows_node = node["flows"].value_or_throw();
    IPathChooser::FlowsTable flows;

    for (ConfigNode node : flows_node.get_node()) {
        ConfigNodeWithPreset flow_node(node, flows_node.get_presets_node());

        std::shared_ptr<IFlow> flow = parse_i_flow(flow_node, endpoints);
        Id flow_id = flow->get_id();
        if (!flows.emplace(flow_id, flow).second) {
            throw flows_node.create_parsing_error(
                fmt::format("Two flows with same name: {}", flow_id));
        }
    }

    return std::make_unique<RoundRobinPathChooser>(std::move(flows));
}

}  // namespace sim
