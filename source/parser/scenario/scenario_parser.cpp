#include "scenario_parser.hpp"

#include "action/action_parser.hpp"
#include "parser/config_reader/config_node.hpp"
#include "parser/network/network_parser.hpp"
#include "parser/relative_path_parser.hpp"

namespace sim {

NewScenario parse_scenario(const std::filesystem::path& path) {
    ConfigNode node = load_file(path);

    std::string network_config_path =
        parse_relative_path(node, "network_config_path", path.parent_path());

    Network network = parse_network(network_config_path);

    ConfigNode actions_node = node["scenario"].value_or_throw();

    Actions actions;

    for (const auto& action_node : actions_node) {
        actions.emplace_back(
            parse_action(action_node, network.get_context().connections_table));
    }

    return NewScenario(std::move(actions), std::move(network));
}

}  // namespace sim
