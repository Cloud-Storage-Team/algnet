#pragma once
#include "connection/i_new_connection.hpp"
#include "scenario/scenario.hpp"
#include "topology/topology.hpp"
#include "utils/id_table.hpp"

namespace sim {
struct SimulationContext {
    Topology topology;
    utils::IdTable<INewConnection> connections_table;
};

class Network {
public:
    explicit Network(SimulationContext a_ctx);

    const SimulationContext& get_context() const;

private:
    SimulationContext m_ctx;
};
}  // namespace sim
