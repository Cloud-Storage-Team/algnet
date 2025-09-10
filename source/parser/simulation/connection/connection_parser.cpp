#include "parser/simulation/connection/connection_parser.hpp"

#include "connection/connection_impl.hpp"
#include "connection/mplb/mplb_factory.hpp"
#include "parser/parse_utils.hpp"
#include "parser/simulation/flow/flow_parser.hpp"

namespace sim {

std::shared_ptr<IConnection> ConnectionParser::parse_i_connection(
    const YAML::Node& key_node, const YAML::Node& value_node) {
    return parse_connection(key_node, value_node);
}

std::shared_ptr<IConnection> ConnectionParser::parse_connection(
    const YAML::Node& key_node, const YAML::Node& value_node) {
    Id conn_id = key_node.as<Id>();
    if (conn_id.empty()) {
        throw std::runtime_error("Connection ID is not specified");
    }

    Id sender_id = value_node["sender_id"].as<Id>();
    if (sender_id.empty()) {
        throw std::runtime_error("Sender ID is not specified for connection " +
                                 conn_id);
    }

    Id receiver_id = value_node["receiver_id"].as<Id>();
    if (receiver_id.empty()) {
        throw std::runtime_error(
            "Receiver ID is not specified for connection " + conn_id);
    }

    std::shared_ptr<IHost> sender_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(sender_id);

    std::shared_ptr<IHost> receiver_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(receiver_id);

    std::uint64_t packets = value_node["packets_to_send"].as<std::uint64_t>();
    if (packets == 0) {
        throw std::runtime_error(
            "Number of packets to send is not specified for connection " +
            conn_id);
    }

    std::string mplb_name = value_node["mplb"].as<std::string>();
    if (mplb_name.empty()) {
        throw std::runtime_error(
            "MPLB algorithm is not specified for connection " + conn_id);
    }
    auto mplb = make_mplb(mplb_name);

    auto conn = std::make_shared<ConnectionImpl>(
        conn_id, sender_ptr, receiver_ptr, std::move(mplb), packets);

    auto& idf = IdentifierFactory::get_instance();
    idf.add_object(conn);

    YAML::Node flows = value_node["flows"];
    if (!flows) {
        throw std::runtime_error("No flows specified for connection " +
                                 conn_id);
    }

    for (auto it = flows.begin(); it != flows.end(); ++it) {
        const YAML::Node& flow_key = it->first;
        const YAML::Node& flow_value = it->second;

        auto flow = FlowParser::parse_i_flow(flow_key, flow_value, conn_id);

        idf.add_object(flow);
        conn->add_flow(flow);
    }

    return conn;
}

}  // namespace sim
