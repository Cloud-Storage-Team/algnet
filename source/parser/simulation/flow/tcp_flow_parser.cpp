#include "parser/simulation/flow/flow_parser.hpp"

#include "flow/tcp/basic/basic_cc.hpp"
#include "flow/tcp/tahoe/tcp_tahoe_cc.hpp"
#include "flow/tcp/swift/swift_cc.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

std::unique_ptr<ITcpCC> ParseFlow::ParseTcpCC::parse_i_tcp_cc(const YAML::Node& cc_node, Id flow_id) {
    if (!cc_node["type"]) {
        throw std::runtime_error("Missing 'cc.type' field in flow " + flow_id);
    }

    std::string type = cc_node["type"].as<std::string>();
    if (type == "basic") {
        return std::make_unique<BasicCC>();
    } else if (type == "tahoe") {
        return std::make_unique<TcpTahoeCC>();
    } else if (type == "swift") {
        TimeNs a_base_target = parse_time(cc_node["base_target"].as<std::string>());
        return std::make_unique<TcpSwiftCC>(a_base_target);
    }
    throw std::runtime_error(fmt::format("Unexpected type of CC module: {}", type));
}

std::shared_ptr<TcpFlow> ParseFlow::tcp_flow_parser(const YAML::Node& key_node,
                                            const YAML::Node& value_node) {
    Id id = key_node.as<Id>();
    if (!value_node["cc"]) {
        throw std::runtime_error("Missing 'cc' field in flow " + id);
    }
    std::unique_ptr<ITcpCC> cc = ParseTcpCC::parse_i_tcp_cc(value_node["cc"], id);

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
