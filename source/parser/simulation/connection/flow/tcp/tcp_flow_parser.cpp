#include "tcp_flow_parser.hpp"

#include "cc/tcp_cc_parser.hpp"
#include "connection/i_connection.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

std::shared_ptr<TcpFlow> TcpFlowParser::parse_tcp_flow(const ConfigNode& node,
                                                       Id conn_id) {
    Id flow_id = node.get_name().value() + "_" + conn_id;
    std::unique_ptr<ITcpCC> cc =
        TcpCCParser::parse_i_tcp_cc(node["cc"].value_or_throw());

    SizeByte packet_size = SizeByte(parse_size(node["packet_size"]
                                                   .value_or_throw()
                                                   .as<std::string>()
                                                   .value_or_throw()));

    std::shared_ptr<IConnection> conn =
        IdentifierFactory::get_instance().get_object<IConnection>(conn_id);

    return std::make_shared<TcpFlow>(flow_id, conn, std::move(cc), packet_size);
}

}  // namespace sim
