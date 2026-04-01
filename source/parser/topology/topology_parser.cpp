#include "topology_parser.hpp"

#include "incast_topology_parser.hpp"
#include "manual_topology_parser.hpp"

namespace sim {

Topology parse_topology(const ConfigNode& node) {
    std::string type = node["type"].as<std::string>().value_or_throw();
    if (type == "custom") {
        return parse_manual_topology(node);
    } else if (type == "incast") {
        return parse_incast_topology(node);
    }
    throw node.create_parsing_error(
        fmt::format("Unexpected topology type: {}", type));
}

}  // namespace sim