#include "scenario_parser.hpp"

#include "action/action_parser.hpp"

namespace sim {

Scenario ScenarioParser::parse(const ConfigNode& scenario_node) {
    if (!scenario_node.IsSequence()) {
        throw scenario_node.create_parsing_error("Node should be a sequence");
    }

    auto scenario = Scenario();

    ActionParser action_parser(scenario.get_summary());

    for (const auto& node : scenario_node) {
        scenario.add_action(action_parser.parse(node));
    }
    return scenario;
}

}  // namespace sim
