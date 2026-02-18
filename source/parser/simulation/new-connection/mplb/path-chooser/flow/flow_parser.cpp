#include "flow_parser.hpp"

#include "connection/flow/tcp/new_tcp_flow.hpp"
#include "parser/parse_utils.hpp"

namespace sim {
std::shared_ptr<sim::NewTcpFlow> parse_tcp_flow(
    const ConfigNodeWithPreset& flow_node, Endpoints enpoints);

std::shared_ptr<INewFlow> parse_i_flow(const ConfigNodeWithPreset& flow_node,
                                       Endpoints endpoints) {
    std::string type =
        flow_node["type"].value_or_throw().as_or_throw<std::string>();
    if (type == "tcp") {
        return parse_tcp_flow(flow_node, std::move(endpoints));
    }
    throw flow_node.create_parsing_error(
        fmt::format("Unsupported flow type: {}", type));
}

std::shared_ptr<sim::NewTcpFlow> parse_tcp_flow(
    const ConfigNodeWithPreset& flow_node, Endpoints endpoints) {
    // TODO: add RTO & ecn_capable parsing (I am too lazy, sorry)
    return NewTcpFlow::create_shared(flow_node.get_name_or_throw(),
                                     endpoints.sender, endpoints.receiver);
}

}  // namespace sim
