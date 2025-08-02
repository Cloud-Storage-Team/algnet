#include "flow/tcp/tcp_flow.hpp"
#include "identifiable_parser.hpp"

namespace sim {
    
template <>
std::shared_ptr<IFlow> Parser<IFlow>::parse_object(const YAML::Node& key_node,
                                                 const YAML::Node& value_node) {
    std::string type = value_node["type"].as<std::string>();
    if (type == "tcp") {
        return Parser<TcpFlow>::parse_object(key_node, value_node);
    }
    throw "wow";
}
}  // namespace sim
