#include "network_parser.hpp"

namespace sim {

Network parse_network(const ConfigNode& node) {
    std::string topology_config_path = node["topology_config_path"]
                                           .value_or_throw()
                                           .as_or_throw<std::string>();

    const ConfigNode& connections_node = node["connections"].value_or_throw();
    
}

}  // namespace sim
