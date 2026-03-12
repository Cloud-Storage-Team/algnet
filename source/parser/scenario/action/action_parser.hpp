#pragma once
#include "network/connection/i_new_connection.hpp"
#include "parser/config_reader/config_node.hpp"
#include "scenario/action/i_action.hpp"
#include "utils/id_table.hpp"

namespace sim {

std::shared_ptr<IAction> parse_action(
    const ConfigNode& node,
    const utils::IdTable<IConnection> connections_table);
}  // namespace sim
