#include "parser/simulation/flow/flow_parser.hpp"

namespace sim {
    
std::shared_ptr<IFlow> FlowParser::parse_i_flow(const YAML::Node& key_node,
                                                 const YAML::Node& value_node,
                                                Id sender_id,
                                                Id receiver_id) {
    std::string type = value_node["type"].as<std::string>();
    if (type == "tcp") {
        return parse_tcp_flow(key_node, value_node, sender_id, receiver_id);
    }
    throw std::runtime_error(fmt::format("Unexpected flow type {}", type));
}

}  // namespace sim
