#include "parser/simulation/flow/flow_parser.hpp"

#include "flow/tcp/basic/basic_cc.hpp"
#include "flow/tcp/tahoe/tcp_tahoe_cc.hpp"
#include "flow/tcp/swift/swift_cc.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

std::unique_ptr<ITcpCC> FlowParser::TcpCCParser::parse_i_tcp_cc(const YAML::Node& cc_node, Id flow_id) {
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

std::shared_ptr<TcpFlow> FlowParser::parse_tcp_flow(const YAML::Node& key_node,
                                            const YAML::Node& value_node, Id sender_id,
                                            Id receiver_id) {
    Id id = key_node.as<Id>();
    if (!value_node["cc"]) {
        throw std::runtime_error("Missing 'cc' field in flow " + id);
    }
    std::unique_ptr<ITcpCC> cc = TcpCCParser::parse_i_tcp_cc(value_node["cc"], id);

    std::shared_ptr<IHost> sender_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(sender_id);

    std::shared_ptr<IHost> receiver_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(receiver_id);


    if (!value_node["packet_size"]) {
        throw std::runtime_error("Flow " + id + " missing parameter packet_size");
    }

    SizeByte packet_size =
        SizeByte(value_node["packet_size"].as<uint64_t>());

    return std::make_shared<TcpFlow>(
        id, sender_ptr, receiver_ptr, std::move(cc), packet_size);
}

}  // namespace sim
