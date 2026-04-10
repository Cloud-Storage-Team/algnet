#pragma once
#include <filesystem>

#include "network/network.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {

Network parse_all_to_all_network(const Topology& topology,
                                 const ConfigNodeWithPreset& node);

}
