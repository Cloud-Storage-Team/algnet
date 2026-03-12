#pragma once
#include "network/connection/i_new_connection.hpp"
#include "parser/config_reader/config_node.hpp"
#include "scenario/action/send_data_action.hpp"
#include "utils/id_table.hpp"

namespace sim {

std::shared_ptr<SendDataAction> parse_send_data_action(
    const ConfigNode& node,
    const utils::IdTable<IConnection> connections_table);

}
