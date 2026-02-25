#include "topology.hpp"

#include "utils/algorithms.hpp"

namespace sim {
Topology::Topology(TopologyContext a_ctx) : m_ctx(std::move(a_ctx)) {}

const TopologyContext& Topology::get_context() const { return m_ctx; }

void Topology::recalculate_paths() const {
    auto process_device = [](std::shared_ptr<IRoutingDevice> src_device) {
        RoutingTable routing_table = bfs(src_device);
        for (const auto& [dest_device_id, links] : routing_table) {
            for (const auto& [link, paths_count] : links) {
                src_device->update_routing_table(dest_device_id, link.lock(),
                                                 paths_count);
            }
        }
    };

    for (auto& [id, host] : m_ctx.hosts_table) {
        process_device(host);
    }

    for (auto& [id, swtch] : m_ctx.switches_table) {
        process_device(swtch);
    }
}

}  // namespace sim
