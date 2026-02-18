#pragma once
#include "connection/mplb/i_new_mplb.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {

std::shared_ptr<INewMPLB> parse_new_mplb(const ConfigNodeWithPreset& node,
                                         Endpoints endpoints);

}
