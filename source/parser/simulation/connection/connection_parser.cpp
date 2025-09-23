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

    YAML::Node data_node = value_node["data_to_send"];
    if (!data_node) {
        throw std::runtime_error("Data to send is not specified for " +
                                 conn_id);
    }
    std::vector<ConnectionImpl::ScheduledChunk> schedule;
    // Backward compatibility when we specify only the initial value in bytes
    if (data_node.IsScalar()) {
        schedule.push_back(
            {TimeNs(0), SizeByte(parse_size(data_node.as<std::string>()))});
        // New format with schedule of chunks
    } else if (data_node.IsSequence()) {
        for (const auto& item : data_node) {
            if (!item["at"] || !item["amount"]) {
                throw std::runtime_error(
                    "Each chunk in data_to_send must have 'at' and 'amount' "
                    "fields for " +
                    conn_id);
            }
            schedule.push_back(
                {parse_time(item["at"].as<std::string>()),
                 SizeByte(parse_size(item["amount"].as<std::string>()))});
        }
    } else {
        throw std::runtime_error("Unsupported data_to_send format for " +
                                 conn_id);
    }

    std::string mplb_name = value_node["mplb"].as<std::string>();
    if (mplb_name.empty()) {
        throw std::runtime_error(
            "MPLB algorithm is not specified for connection " + conn_id);
    }
    auto mplb = make_mplb(mplb_name);

    auto conn = std::make_shared<ConnectionImpl>(conn_id, sender_ptr,
                                                 receiver_ptr, std::move(mplb));

    conn->set_data_schedule(std::move(schedule));

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
