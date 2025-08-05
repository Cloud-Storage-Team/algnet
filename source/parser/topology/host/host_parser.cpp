#include "host_parser.hpp"

namespace sim {
    
std::shared_ptr<IHost> HostParser::parse_i_host(const YAML::Node& key_node,
                                                 const YAML::Node& value_node) {
    return parse_default_host(key_node, value_node);
}

std::shared_ptr<Host> HostParser::parse_default_host(const YAML::Node& key_node,
                                            [[maybe_unused]] const YAML::Node& value_node) {
    return std::make_shared<Host>(key_node.as<Id>());                                              
}

}  // namespace sim
