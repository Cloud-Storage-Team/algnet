#include "parse_flow.hpp"

namespace sim {

std::shared_ptr<TcpFlow> ParseFlow::parse_tcp_flow(const YAML::Node& key_node,
                                            const YAML::Node& value_node) {
    std::unique_ptr<ITcpCC> cc = ParseTcpCC::parse_i_tcp_cc(key_node, value_node);
    Id id = key_node.as<Id>();

    if (!value_node["sender_id"]) {
        throw std::runtime_error("Flow " + id + " missing sender");
    }
    if(!value_node["receiver_id"]) {
        throw std::runtime_error("Flow " + id + " missing receiver");
    }

    Id sender_id = value_node["sender_id"].as<Id>();
    std::shared_ptr<IHost> sender_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(sender_id);

    Id receiver_id = value_node["receiver_id"].as<Id>();
    std::shared_ptr<IHost> receiver_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(receiver_id);


    if (!value_node["packet_size"]) {
        throw std::runtime_error("Flow " + id + " missing parameter packet_size");
    }
    if (!value_node["number_of_packets"]) {
        throw std::runtime_error("Flow " + id + " missing parameter number_of_packets");
    }

    SizeByte packet_size =
        SizeByte(value_node["packet_size"].as<uint64_t>());
    std::uint32_t number_of_packets =
        value_node["number_of_packets"].as<std::uint32_t>();

    return std::make_shared<TcpFlow>(
        id, sender_ptr, receiver_ptr, std::move(cc), packet_size, number_of_packets);
}

}  // namespace sim
