#include "parser/simulation/connection/connection_parser.hpp"

#include "connection/connection_impl.hpp"
#include "connection/mplb/mplb_factory.hpp"
#include "flow/flow_parser.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

std::shared_ptr<IConnection> ConnectionParser::parse_i_connection(
    const ConfigNode& node) {
    return parse_connection(node);
}

std::shared_ptr<IConnection> ConnectionParser::parse_connection(
    const ConfigNode& node) {
    Id conn_id = node.get_name().value();

    Id sender_id =
        node["sender_id"].value_or_throw().as<std::string>().value_or_throw();

    Id receiver_id =
        node["receiver_id"].value_or_throw().as<Id>().value_or_throw();

    std::shared_ptr<IHost> sender_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(sender_id);

    std::shared_ptr<IHost> receiver_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(receiver_id);

    std::string mplb_name =
        node["mplb"].value_or_throw().as<std::string>().value_or_throw();

    auto mplb = make_mplb(mplb_name);

    auto conn = std::make_shared<ConnectionImpl>(conn_id, sender_ptr,
                                                 receiver_ptr, std::move(mplb));

    auto& idf = IdentifierFactory::get_instance();
    idf.add_object(conn);

    ConfigNode flows_node = node["flows"].value_or_throw();

    for (auto it = flows_node.begin(); it != flows_node.end(); ++it) {
        ConfigNode flow_node = *it;

        std::shared_ptr<IFlow> flow(
            FlowParser::parse_i_flow(flow_node, conn_id));

        idf.add_object(flow);
        conn->add_flow(flow);
    }

    return conn;
}

}  // namespace sim
