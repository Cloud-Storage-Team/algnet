#include "parser/simulation/flow/parse_flow.hpp"

#include "flow/tcp/basic/basic_cc.hpp"
#include "flow/tcp/tahoe/tcp_tahoe_cc.hpp"
#include "flow/tcp/swift/swift_cc.hpp"
#include "parser/parse_utils.hpp"

namespace sim {
    
std::shared_ptr<IFlow> ParseFlow::parse_i_flow(const YAML::Node& key_node,
                                                 const YAML::Node& value_node) {
    std::string type = value_node["type"].as<std::string>();
    if (type == "tcp") {
        return parse_tcp_flow(key_node, value_node);
    }
    throw std::runtime_error(fmt::format("Unexpected flow type {}", type));
}

}  // namespace sim
