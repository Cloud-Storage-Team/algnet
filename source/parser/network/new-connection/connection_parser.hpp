#pragma once
#include "network/connection/i_new_connection.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"
#include "topology/device/interfaces/i_host.hpp"
#include "utils/id_table.hpp"

namespace sim {

std::shared_ptr<INewConnection> parse_i_connection(
    const ConfigNodeWithPreset& node, const utils::IdTable<IHost>& hosts_table);

}  // namespace sim
