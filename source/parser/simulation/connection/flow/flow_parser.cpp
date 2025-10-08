#include "flow_parser.hpp"

#include "tcp/tcp_flow_parser.hpp"
namespace sim {

std::shared_ptr<IFlow> FlowParser::parse_i_flow(const ConfigNode& node,
                                                Id conn_id) {
    std::string type =
        node["type"].value_or_throw().as<std::string>().value_or_throw();
    if (type == "tcp") {
        return TcpFlowParser::parse_tcp_flow(node, conn_id);
    }
    throw std::runtime_error(fmt::format("Unexpected flow type {}", type));
}

}  // namespace sim
