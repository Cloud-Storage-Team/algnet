#pragma once
#include "connection/i_new_connection.hpp"
#include "new_scenario/new_action/send_data_action.hpp"
#include "parser/config_reader/config_node.hpp"
#include "utils/id_table.hpp"

namespace sim {

std::shared_ptr<NewSendDataAction> parse_send_data_action(
    const ConfigNode& node,
    const utils::IdTable<INewConnection> connections_table);

}