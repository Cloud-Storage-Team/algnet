#pragma once

#include "action_handler.hpp"
#include <memory>

namespace sim {

class SendDataConnHandler final : public IScenarioActionHandler {
public:
    void handle(const YAML::Node& node, ScenarioImpl& scenario) override;
};

}  // namespace sim
