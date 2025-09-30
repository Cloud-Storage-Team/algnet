#include "scenario_parser.hpp"

#include "action/action_parser.hpp"

namespace sim {

Scenario ScenarioParser::parse(const YAML::Node& scenario_node) {
    if (!scenario_node || !scenario_node.IsSequence()) {
        throw std::runtime_error("`scenario` must be a sequence");
    }

    auto scenario = Scenario();

    for (const auto& node : scenario_node) {
        scenario.add_action(ActionParser::parse(node));
    }
    return scenario;
}

}  // namespace sim
