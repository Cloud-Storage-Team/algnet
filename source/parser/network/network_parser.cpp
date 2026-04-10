#include "network_parser.hpp"

#include "all-to-all_network_parser.hpp"
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

    std::string type = node["type"].value_or_throw().as_or_throw<std::string>();
    if (type == "custom") {
        return parse_custom_network(topology, node.get_node());
    } else if (type == "all-to-all") {
        return parse_all_to_all_network(topology, node);
    }
    throw node.create_parsing_error(
        fmt::format("Undefined network type: {}", type));
}

}  // namespace sim
