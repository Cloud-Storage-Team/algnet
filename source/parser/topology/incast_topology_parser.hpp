#pragma once
#include "parser/config_reader/config_node_with_preset.hpp"
#include "topology/topology.hpp"

namespace sim {

// Parses incast topology.
Topology parse_incast_topology(const ConfigNode& node);

}  // namespace sim
