#include "parse_flow_common.hpp"

namespace sim {

FlowCommon parse_flow_common(const YAML::Node& key_node,
                             const YAML::Node& value_node) {
    FlowCommon result;
    result.id = key_node.as<Id>();

    Id sender_id = value_node["sender_id"].as<Id>();
    result.sender_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(sender_id);

    Id receiver_id = value_node["receiver_id"].as<Id>();
    result.receiver_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(receiver_id);

    result.packet_size = value_node["packet_size"].as<Size>();
    result.number_of_packets =
        value_node["number_of_packets"].as<std::uint32_t>();
    result.packet_interval = value_node["packet_interval"].as<Time>();

    return result;
}

}  // namespace sim