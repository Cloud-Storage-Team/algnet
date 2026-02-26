#pragma once
#include "new_scenario/new_action/stop_time_action.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

std::shared_ptr<NewStopTimeAction> parse_stop_time_action(
    const ConfigNode& node);

}