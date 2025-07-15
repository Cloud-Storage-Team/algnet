#pragma once
#include "flow/tcp/i_tcp_cc.hpp"
#include "flow/tcp/tcp_flow.hpp"
#include "parser/identifiable_parser/identifiable_parser.hpp"
#include "parser/identifiable_parser/parse_flow_common.hpp"

namespace sim {

template <typename TTcpCC>
requires std::derived_from<TTcpCC, ITcpCC>
class IdentifieableParser<TcpFlow<TTcpCC>> {
public:
    static std::shared_ptr<TcpFlow<TTcpCC>> parse_and_registrate(
        const YAML::Node& key_node, const YAML::Node& value_node) {
        TTcpCC cc = parse_tcp_cc(key_node, value_node);
        FlowCommon flow_common = parse_flow_common(key_node, value_node);
        std::shared_ptr<TcpFlow<TTcpCC>> tcp_flow =
            std::make_shared<TcpFlow<TTcpCC>>(
                flow_common.id,
                dynamic_pointer_cast<IHost>(flow_common.sender_ptr),
                dynamic_pointer_cast<IHost>(flow_common.receiver_ptr), cc,
                flow_common.packet_size, flow_common.packet_interval,
                flow_common.number_of_packets);
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
