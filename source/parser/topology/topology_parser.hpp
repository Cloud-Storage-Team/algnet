#pragma once
#include "network/topology/topology.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {

// Parses topology.
// ATTENTION: thinks that all presets node are placed under
// node["presets"]["hosts"/"switches"/"links"]
Topology parse_topology(const ConfigNode& node);

}  // namespace sim
