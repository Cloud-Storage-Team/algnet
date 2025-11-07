#include "flow_parser.hpp"

#include "tcp/tcp_flow_parser.hpp"
namespace sim {

std::shared_ptr<IFlow> FlowParser::parse_i_flow(const ConfigNode& node,
                                                Id conn_id) {
    std::string type = node["type"].value_or_throw().as_or_throw<std::string>();
    if (type == "tcp") {
        return TcpFlowParser::parse_tcp_flow(node, conn_id);
    }
    throw node.create_parsing_error("Unexpected flow type: " + type);
}

}  // namespace sim
