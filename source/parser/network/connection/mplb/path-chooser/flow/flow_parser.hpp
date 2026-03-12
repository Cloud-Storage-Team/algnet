#pragma once
#include "network/connection/flow/i_new_flow.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {

std::shared_ptr<IFlow> parse_i_flow(const ConfigNodeWithPreset& flow_node,
                                    Endpoints endpoints);

}  // namespace sim
