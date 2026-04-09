#pragma once
#include "network/connection/i_connection.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"
#include "topology/device/interfaces/i_host.hpp"
#include "utils/id_table.hpp"

namespace sim {

std::shared_ptr<IConnection> parse_i_connection(
    const ConfigNodeWithPreset& node, const utils::IdTable<IHost>& hosts_table);


std::shared_ptr<IConnection> parse_i_connection(
    std::shared_ptr<IHost> sender, std::shared_ptr<IHost> receiver,
    const ConfigNodeWithPreset& params);

std::shared_ptr<IConnection> parse_i_connection(
    std::shared_ptr<IHost> sender, std::shared_ptr<IHost> receiver,
    const ConfigNodeWithPreset& params, const Id& connection_id);


}  // namespace sim
