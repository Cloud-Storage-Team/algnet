#pragma once

#include <memory>

#include "parser/config_node.hpp"
#include "scenario/scenario.hpp"

namespace sim {

class ScenarioParser {
public:
    static Scenario parse(const YAML::Node& scenario_node);
};

}  // namespace sim
