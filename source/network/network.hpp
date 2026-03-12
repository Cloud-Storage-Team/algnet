#pragma once
#include "network/connection/i_new_connection.hpp"
#include "topology/topology.hpp"
#include "utils/id_table.hpp"

namespace sim {
struct NetworkContext {
    Topology topology;
    utils::IdTable<INewConnection> connections_table;
};

// Tetwork itself: topolog & connections
class Network {
public:
    explicit Network(NetworkContext a_ctx);

    void recalculate_pathes();

    void save_metrics(std::filesystem::path output_dir) const;

    const NetworkContext& get_context() const;

private:
    NetworkContext m_ctx;
};
}  // namespace sim
