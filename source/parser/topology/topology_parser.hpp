#pragma once
#include "parser/config_reader/config_node_with_preset.hpp"
#include "topology/topology.hpp"

namespace sim {

// Parses topology.
Topology parse_topology(const ConfigNodeWithPreset& node);

}  // namespace sim
