#include "network.hpp"

#include "metrics/metrics_table/combine_metrics_tables.hpp"

namespace sim {

Network::Network(NetworkContext a_ctx) : m_ctx(std::move(a_ctx)) {}

void Network::recalculate_pathes() { m_ctx.topology.recalculate_paths(); }

void Network::save_metrics(std::filesystem::path output_dir) const {
    collect_and_save_all_metrics(m_ctx.connections_table,
                                 output_dir / "connections");
    std::filesystem::path link_metrics_path = output_dir / "links";
    for (const auto& [link_id, link] :
         m_ctx.topology.get_context().links_table) {
        link->write_all_metrics(link_metrics_path / link_id);
    }
}

const NetworkContext& Network::get_context() const { return m_ctx; }
}  // namespace sim
