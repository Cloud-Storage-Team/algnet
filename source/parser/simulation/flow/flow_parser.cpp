#include "parser/simulation/flow/flow_parser.hpp"

namespace sim {
    
std::shared_ptr<IFlow> ParseFlow::parse_i_flow(const YAML::Node& key_node,
                                                 const YAML::Node& value_node) {
    std::string type = value_node["type"].as<std::string>();
    if (type == "tcp") {
        return tcp_flow_parser(key_node, value_node);
    }
    throw std::runtime_error(fmt::format("Unexpected flow type {}", type));
}

}  // namespace sim
