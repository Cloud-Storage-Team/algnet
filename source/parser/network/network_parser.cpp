#include "network_parser.hpp"

#include "new-connection/connection_parser.hpp"
#include "parser/relative_path_parser.hpp"
#include "parser/topology/topology_parser.hpp"

namespace sim {

Network parse_network(const std::filesystem::path& path) {
    ConfigNode node = load_file(path);

    std::string topology_config_path =
        parse_relative_path(node, "topology_config_path", path.parent_path());

    const ConfigNode topology_config = load_file(topology_config_path);

    Topology topology = parse_topology(topology_config);

    const ConfigNode& connections_node = node["connections"].value_or_throw();

    NetworkContext ctx(std::move(topology), utils::IdTable<IConnection>{});

    std::optional<ConfigNode> presets_node = node["presets"].to_optional();

    for (const auto& connection_node : connections_node) {
        ConfigNodeWithPreset conn_node_with_preset(connection_node,
                                                   presets_node);

        std::shared_ptr<IConnection> connection = parse_i_connection(
            conn_node_with_preset, ctx.topology.get_context().hosts_table);
        Id conn_id = connection->get_id();
        if (!ctx.connections_table.emplace(conn_id, connection).second) {
            throw connections_node.create_parsing_error(
                fmt::format("Two connection with same name: {}", conn_id));
        }
    }

    return Network{std::move(ctx)};
}

}  // namespace sim
