#pragma once
#include <yaml-cpp/yaml.h>

#include <memory>

#include "scenario/i_scenario.hpp"

namespace sim {

class ScenarioParser {
public:
    // Parse the scenario node and return the implementation with filled actions
    static std::shared_ptr<IScenario> parse(const YAML::Node& scenario_node);
};

}  // namespace sim
