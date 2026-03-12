#pragma once
#include "connection/i_new_connection.hpp"
#include "parser/config_reader/config_node.hpp"
#include "scenario/new_action/i_new_action.hpp"
#include "utils/id_table.hpp"

namespace sim {

std::shared_ptr<INewAction> parse_action(
    const ConfigNode& node,
    const utils::IdTable<INewConnection> connections_table);
}  // namespace sim
