#pragma once
#include "connection/i_new_connection.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {

std::shared_ptr<INewConnection> parse_i_connection(const ConfigNodeWithPreset& node);

}  // namespace sim
