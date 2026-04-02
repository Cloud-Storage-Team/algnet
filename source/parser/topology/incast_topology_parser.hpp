#pragma once
#include "parser/config_reader/config_node_with_preset.hpp"
#include "topology/topology.hpp"

namespace sim {

// Parses incast topology.
Topology parse_incast_topology(const ConfigNodeWithPreset& node);

struct IncastTopologyContext {
    utils::IdTable<IHost> senders;
    std::shared_ptr<IHost> receiver;
    std::shared_ptr<ISwitch> swtch;

    utils::IdTable<ILink> links;

    TopologyContext to_topology_context() const;
};

IncastTopologyContext parse_incast_topology_context(
    const ConfigNodeWithPreset& node);

}  // namespace sim
