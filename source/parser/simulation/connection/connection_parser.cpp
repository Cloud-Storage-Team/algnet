#include "connection/mplb_factory.hpp"
#include "parser/parse_utils.hpp"
#include "parser/simulation/connection/connection_parser.hpp"
#include "parser/simulation/flow/flow_parser.hpp"

namespace sim {

std::shared_ptr<Connection> ConnectionParser::parse_connection(
    const YAML::Node& key_node, const YAML::Node& value_node) {
    Id conn_id = key_node.as<Id>();
    Id sender_id = value_node["sender_id"].as<Id>();
    Id receiver_id = value_node["receiver_id"].as<Id>();
    std::uint64_t packets = value_node["packets_to_send"].as<std::uint64_t>();

    std::string mplb_name = value_node["mplb"].as<std::string>();
    auto policy = make_mplb(mplb_name);

    auto conn = std::make_shared<Connection>(
        conn_id, sender_id, receiver_id,
        SizeByte(0),
        std::move(policy), packets);

    YAML::Node flows_node = value_node["flows"];
    auto& idf = IdentifierFactory::get_instance();

    for (auto it = flows_node.begin(); it != flows_node.end(); ++it) {
        const YAML::Node& flow_key = it->first;
        const YAML::Node& flow_value = it->second;

        auto flow = FlowParser::parse_i_flow(flow_key, flow_value);

        idf.add_object(flow);
        flow->set_conn(conn);
        conn->add_flow(flow);
    }

    return conn;
}

}  // namespace sim
