#include "parser/simulator/flow/parse_flow.hpp"

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


std::unique_ptr<ITcpCC> ParseFlow::ParseTcpCC::parse_i_tcp_cc(Id flow_id, const YAML::Node& value_node) {
    if (!value_node["type"]) {
        throw std::runtime_error("Missing 'cc.type' field in flow " + flow_id);
    }

    std::string type = value_node["cc"]["type"].as<std::string>();
    if (type == "basic") {
        return std::make_unique<BasicCC>();
    } else if (type == "tahoe") {
        return std::make_unique<TcpTahoeCC>();
    } else if (type == "swift") {
        TimeNs a_base_target = parse_time(value_node["cc"]["base_target"].as<std::string>());
        return std::make_unique<TcpSwiftCC>(a_base_target);
    }
    throw std::runtime_error(fmt::format("Unexpected type of CC module: {}", type));
}

}  // namespace sim
