#pragma once

#include <yaml-cpp/yaml.h>
#include "scenario/scenario_impl.hpp"

namespace sim {

class IScenarioActionHandler {
public:
    virtual ~IScenarioActionHandler() = default;
    virtual void handle(const YAML::Node& node, ScenarioImpl& scenario) = 0;
};

}  // namespace sim