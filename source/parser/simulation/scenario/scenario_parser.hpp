#pragma once

#include <yaml-cpp/yaml.h>

#include <memory>

#include "scenario/scenario.hpp"

namespace sim {

class ScenarioParser {
public:
    static Scenario parse(const YAML::Node& scenario_node);
};

}  // namespace sim
