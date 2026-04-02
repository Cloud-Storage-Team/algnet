#pragma once
#include "common.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"
#include "topology/topology.hpp"

namespace sim {

// Parses incast topology.
Topology parse_fat_tree_topology(const ConfigNodeWithPreset& node);

// More detailed parsers

struct FatTreeTopologyContext {
    struct Switches {
        Storage<ISwitch> core;
        Storage<ISwitch> aggr;
        Storage<ISwitch> edge;
    } switches;

    Storage<IHost> hosts;

    struct Links {
        Storage<ILink> core_aggr;
        Storage<ILink> aggr_edge;
        Storage<ILink> edge_host;
    } links;

    TopologyContext to_topology_context() const;
};

struct FatTreeParams {
    using NodeRef = const ConfigNodeWithPreset&;

    NodeRef packet_spraying;

    // stored as NodeRef to have ability to throw error using this ref
    NodeRef switch_ports;

    NodeRef core_switches;
    NodeRef aggr_switches;
    NodeRef edge_switches;

    NodeRef hosts;

    NodeRef core_aggr_links;
    NodeRef aggr_edge_links;
    NodeRef edge_host_links;
};

FatTreeTopologyContext parse_fat_tree_topology_context(
    const ConfigNodeWithPreset& node);

FatTreeTopologyContext create_fat_tree_topology_context(FatTreeParams params);

}  // namespace sim
