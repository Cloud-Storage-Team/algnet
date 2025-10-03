#include "tcp_flow_parser.hpp"

#include "cc/tcp_cc_parser.hpp"
#include "connection/i_connection.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

std::shared_ptr<TcpFlow> TcpFlowParser::parse_tcp_flow(
    const YAML::Node& key_node, const YAML::Node& value_node, Id conn_id) {
    Id flow_id = key_node.as<Id>() + "_" + conn_id;
    if (!value_node["cc"]) {
        throw std::runtime_error("Missing 'cc' field in flow " + flow_id);
    }
    std::unique_ptr<ITcpCC> cc = TcpCCParser::parse_i_tcp_cc(value_node["cc"]);

    if (!value_node["packet_size"]) {
        throw std::runtime_error("Flow " + flow_id +
                                 " missing parameter packet_size");
    }

    SizeByte packet_size =
        SizeByte(parse_size(value_node["packet_size"].as<std::string>()));

    std::shared_ptr<IConnection> conn =
        IdentifierFactory::get_instance().get_object<IConnection>(conn_id);

    return std::make_shared<TcpFlow>(flow_id, conn, std::move(cc), packet_size);
}

}  // namespace sim
