#include "network_parser.hpp"

#include "custom_network_parser.hpp"
#include "parser/relative_path_parser.hpp"
#include "parser/topology/topology_parser.hpp"

namespace sim {

Network parse_network(const std::filesystem::path& path) {
    ConfigNodeWithPreset node = load_file_with_presets(path);

    std::string topology_config_path = parse_relative_path(
        node.get_node(), "topology_config_path", path.parent_path());

    const ConfigNodeWithPreset topology_config =
        load_file_with_presets(topology_config_path);

    Topology topology = parse_topology(topology_config);

    return parse_custom_network(topology, node.get_node());
}

}  // namespace sim
