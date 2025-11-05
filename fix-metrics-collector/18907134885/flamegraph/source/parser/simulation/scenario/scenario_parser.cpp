#include "scenario_parser.hpp"

#include "action/action_parser.hpp"

namespace sim {

Scenario ScenarioParser::parse(const ConfigNode& scenario_node) {
    if (!scenario_node.IsSequence()) {
        throw scenario_node.create_parsing_error("Node should be a sequence");
    }

    auto scenario = Scenario();

    for (const auto& node : scenario_node) {
        scenario.add_action(ActionParser::parse(node));
    }
    return scenario;
}

}  // namespace sim
