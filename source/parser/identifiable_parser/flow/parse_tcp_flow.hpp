#pragma once
#include "flow/tcp/i_tcp_cc.hpp"
#include "flow/tcp/tcp_flow.hpp"
#include "parser/identifiable_parser/identifiable_parser.hpp"

namespace sim {

template <typename TTcpCC>
requires std::derived_from<TTcpCC, ITcpCC>
class Parser<TcpFlow<TTcpCC>> {
public:
    static std::shared_ptr<TcpFlow<TTcpCC>> parse_object(
        const YAML::Node& key_node, const YAML::Node& value_node) {
        TTcpCC cc = parse_tcp_cc(key_node, value_node);
        Id id = key_node.as<Id>();

        Id sender_id = value_node["sender_id"].as<Id>();
        std::shared_ptr<IHost> sender_ptr =
            IdentifierFactory::get_instance().get_object<IHost>(sender_id);

        Id receiver_id = value_node["receiver_id"].as<Id>();
        std::shared_ptr<IHost> receiver_ptr =
            IdentifierFactory::get_instance().get_object<IHost>(receiver_id);

        SizeByte packet_size =
            SizeByte(value_node["packet_size"].as<uint64_t>());
        std::uint32_t number_of_packets =
            value_node["number_of_packets"].as<std::uint32_t>();

        return std::make_shared<TcpFlow<TTcpCC>>(
            id, sender_ptr, receiver_ptr, cc, packet_size, number_of_packets);
    }

private:
    static TTcpCC parse_tcp_cc(const YAML::Node& key_node,
                               const YAML::Node& value_node);
};

}  // namespace sim
