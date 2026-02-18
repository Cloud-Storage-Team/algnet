#pragma once
#include "connection/flow/i_new_flow.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {

std::shared_ptr<INewFlow> parse_i_flow(const ConfigNodeWithPreset& flow_node,
                                       Endpoints enpoints);

}  // namespace sim
