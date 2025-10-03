#pragma once

#include <memory>

#include "parser/config_node.hpp"
#include "parser/parse_utils.hpp"
#include "scenario/action/i_action.hpp"

namespace sim {

class ActionParser {
public:
    // Parse one YAML node into an IAction
    static std::unique_ptr<IAction> parse(const YAML::Node& node);

private:
    static std::unique_ptr<IAction> parse_send_data(const YAML::Node& node);
};

}  // namespace sim
