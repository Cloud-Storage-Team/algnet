#pragma once

#include <memory>

#include "parser/config_reader/config_node.hpp"
#include "scenario/scenario.hpp"

namespace sim {

class ScenarioParser {
public:
    static Scenario parse(const ConfigNode& scenario_node);
};

}  // namespace sim
