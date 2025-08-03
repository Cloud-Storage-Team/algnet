#include "parse_i_flow_anon.hpp"
#include "flow/tcp/tcp_flow.hpp"

namespace sim {
    
std::shared_ptr<IFlow> parse_i_flow(const YAML::Node& key_node,
                                                 const YAML::Node& value_node) {
    std::string type = value_node["type"].as<std::string>();
    if (type == "tcp") {
        return parse_tcp_flow(key_node, value_node);
    }
    throw "wow";
}

namespace {

    std::unique_ptr<ITcpCC> parse_tcp_cc([[maybe_unused]]const YAML::Node& key_node, const YAML::Node& value_node) {
        std::string type = value_node["cc"]["type"].as<std::string>();
        if (type == "basic") {
            return std::make_unique<BasicCC>();
        } else if (type == "tahoe") {
            return std::make_unique<TcpTahoeCC>();
        } else if (type == "swift") {
            TimeNs a_base_target = parse_time(value_node["cc"]["base_target"].as<std::string>());
            return std::make_unique<TcpSwiftCC>(a_base_target);
        }
        throw "wow";
    }


    std::shared_ptr<TcpFlow> parse_tcp_flow(const YAML::Node& key_node,
                                                const YAML::Node& value_node) {
        std::unique_ptr<ITcpCC> cc = parse_tcp_cc(key_node, value_node);
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

        return std::make_shared<TcpFlow>(
            id, sender_ptr, receiver_ptr, std::move(cc), packet_size, number_of_packets);                                                   
    }
}

}  // namespace sim
