#pragma once
#include "parser/config_reader/config_node_with_preset.hpp"
#include "topology/topology.hpp"

namespace sim {

struct LeafSpineParams {
    using NodeRef = const ConfigNodeWithPreset&;

    NodeRef packet_spraying;

    NodeRef spine_switches;
    NodeRef leaf_switches;

    NodeRef hosts;

    NodeRef leaf_spine_links;
    NodeRef host_leaf_links;
};

// Parses incast topology.
Topology parse_leaf_spine_topology(const ConfigNodeWithPreset& node);

TopologyContext create_leaf_spine_topology_context(
    LeafSpineParams params, const std::string& names_prefix = "");

}  // namespace sim
