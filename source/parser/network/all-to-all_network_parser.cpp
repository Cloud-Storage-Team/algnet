#include "all-to-all_network_parser.hpp"

#include "connection/connection_parser.hpp"

namespace sim {
Network parse_all_to_all_network(const Topology& topology,
                                 const ConfigNodeWithPreset& node) {
    const ConfigNodeWithPreset& connections_params =
        node["connections_params"].value_or_throw();

    NetworkContext ctx(topology, utils::IdTable<IConnection>{});

    const TopologyContext& topology_ctx = topology.get_context();
    for (const auto& [sender_name, sender] : topology_ctx.hosts_table) {
        for (const auto& [receiver_name, receiver] : topology_ctx.hosts_table) {
            if (sender == receiver) {
                continue;
            }
            ctx.connections_table.add(
                parse_i_connection(sender, receiver, connections_params));
        }
    }

    return Network{std::move(ctx)};
}
}  // namespace sim