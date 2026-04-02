#pragma once
#include "parser/config_reader/config_node_with_preset.hpp"
#include "topology/topology.hpp"

namespace sim {

// Parses incast topology.
Topology parse_leaf_spine_topology(const ConfigNodeWithPreset& node);

// More detailed parsers
struct LeafSpineTopologyContext {
    template <typename T>
    using Storage = std::vector<std::shared_ptr<T> >;

    Storage<ISwitch> spine_switches;
    Storage<ISwitch> leaf_switches;
    Storage<IHost> hosts;

    utils::IdTable<ILink> links;

    TopologyContext to_topology_context() const;
};

struct LeafSpineParams {
    using NodeRef = const ConfigNodeWithPreset&;

    NodeRef packet_spraying;

    NodeRef spine_switches;
    NodeRef leaf_switches;

    NodeRef hosts;

    NodeRef leaf_spine_links;
    NodeRef host_leaf_links;
};

LeafSpineTopologyContext parse_leaf_spine_topology_context(
    const ConfigNodeWithPreset& node);

LeafSpineTopologyContext create_leaf_spine_topology_context(
    LeafSpineParams params, const std::string& names_prefix = "");

}  // namespace sim
