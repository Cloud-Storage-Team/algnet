#pragma once
#include "topology/device/interfaces/i_host.hpp"
#include "topology/device/interfaces/i_switch.hpp"
#include "topology/link/i_link.hpp"
#include "utils/id_table.hpp"

namespace sim {

struct TopologyContext {
    utils::IdTable<IHost> hosts_table;
    utils::IdTable<ISwitch> switches_table;
    utils::IdTable<ILink> links_table;
};

// Network topology: hosts, switches & links
class Topology {
public:
    explicit Topology(TopologyContext a_ctx);

    const TopologyContext& get_context() const;

    // Calls BFS for each device to build the routing tables
    void recalculate_paths() const;

private:
    TopologyContext m_ctx;
};

}  // namespace sim
