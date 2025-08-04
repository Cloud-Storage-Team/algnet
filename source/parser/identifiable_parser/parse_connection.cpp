#include <cstdint>
#include <memory>
#include <string>

#include "connection/connection.hpp"
#include "connection/mplb_policy_factory.hpp"
#include "parser/identifiable_parser/identifiable_parser.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

template <>
std::shared_ptr<Connection> IdentifieableParser<Connection>::parse_and_registrate(
    const YAML::Node& key_node, const YAML::Node& cfg) {

    Id conn_id = key_node.as<Id>();
    Id sender_id = cfg["sender_id"].as<Id>();
    Id receiver_id = cfg["receiver_id"].as<Id>();
    std::uint64_t packets = cfg["number_of_packets"].as<std::uint64_t>();
    SizeByte packet_size = SizeByte(cfg["packet_size"].as<uint64_t>());

    std::string alg = cfg["mplb_algorithm"]
                          ? cfg["mplb_algorithm"].as<std::string>()
                          : "round_robin";

    auto policy = make_mplb_policy(alg);
    auto conn = std::make_shared<Connection>(
        conn_id, sender_id, receiver_id, packet_size, policy, packets);
    conn->configure(packets);

    return conn;
}

template <>
std::shared_ptr<Connection> Parser<Connection>::parse_object(
    const YAML::Node& key_node, const YAML::Node& value_node) {
    return IdentifieableParser<Connection>::parse_and_registrate(key_node, value_node);
}

}  // namespace sim
