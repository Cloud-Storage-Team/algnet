#include "path_chooser_parser.hpp"

#include "connection/mplb/path_chooser/round_robin/round_robin_path_chooser.hpp"
#include "parser/simulation/new-connection/mplb/path-chooser/flow/flow_parser.hpp"

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
    IPathChooser::FlowsSet flows;

    // TODO: iterate over flows_node (after its itarator implementation)
    for (ConfigNode node : flows_node.get_node()) {
        ConfigNodeWithPreset flow_node(node, flows_node.get_presets_node());

        std::shared_ptr<INewFlow> flow = parse_i_flow(flow_node, endpoints);
        flows.emplace(flow);
    }

    return std::make_unique<RoundRobinPathChooser>(std::move(flows));
}

}  // namespace sim
