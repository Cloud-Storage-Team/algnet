#pragma once
#include "connection/i_new_connection.hpp"
#include "scenario/scenario.hpp"
#include "topology/topology.hpp"
#include "utils/id_table.hpp"

namespace sim {
struct NetworkContext {
    Topology topology;
    utils::IdTable<INewConnection> connections_table;
};

class Network {
public:
    explicit Network(NetworkContext a_ctx);

    const NetworkContext& get_context() const;

private:
    NetworkContext m_ctx;
};
}  // namespace sim
