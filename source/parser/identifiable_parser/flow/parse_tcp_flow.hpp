#pragma once
#include "flow/tcp/i_tcp_cc.hpp"
#include "flow/tcp/tcp_flow.hpp"
#include "parser/identifiable_parser/identifiable_parser.hpp"

namespace sim {

template <typename TTcpCC>
requires std::derived_from<TTcpCC, ITcpCC>
class IdentifieableParser<TcpFlow<TTcpCC>> {
public:
    static std::shared_ptr<TcpFlow<TTcpCC>> parse_and_registrate(
        const YAML::Node& key_node, const YAML::Node& value_node) {
        TTcpCC cc = parse_tcp_cc(key_node, value_node);
        Id id = key_node.as<Id>();

        Id sender_id = value_node["sender_id"].as<Id>();
        std::shared_ptr<IHost> sender_ptr =
            IdentifierFactory::get_instance().get_object<IHost>(sender_id);

        Id receiver_id = value_node["receiver_id"].as<Id>();
        std::shared_ptr<IHost> receiver_ptr =
            IdentifierFactory::get_instance().get_object<IHost>(receiver_id);

        Size packet_size = value_node["packet_size"].as<Size>();
        std::uint32_t number_of_packets =
            value_node["number_of_packets"].as<std::uint32_t>();
        Time packet_interval = value_node["packet_interval"].as<Time>();

        std::shared_ptr<TcpFlow<TTcpCC>> tcp_flow =
            std::make_shared<TcpFlow<TTcpCC>>(id, sender_ptr, receiver_ptr, cc,
                                              packet_size, packet_interval,
                                              number_of_packets);
        if (!IdentifierFactory::get_instance().add_object(tcp_flow)) {
            throw std::runtime_error(fmt::format(
                "Can not add object with type {}; object with same id ({}) "
                "already exists",
                typeid(TcpFlow<TTcpCC>).name(), tcp_flow.get()->get_id()));
        }
        return tcp_flow;
    }

private:
    static TTcpCC parse_tcp_cc(const YAML::Node& key_node,
                               const YAML::Node& value_node);
};

}  // namespace sim
