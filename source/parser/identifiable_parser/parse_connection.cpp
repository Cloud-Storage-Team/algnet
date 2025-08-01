#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "connection/connection.hpp"
#include "connection/mplb_policy_factory.hpp"
#include "identifiable_parser/identifiable_parser.hpp"

namespace sim {

template <>
class IdentifieableParser<Connection> {
public:
    // key_node: YAML key (connection ID)
    // cfg     : YAML node with fields sender_id, receiver_id,
    // number_of_packets, mplb_algorithm
    static std::shared_ptr<Connection> parse_and_registrate(
        const YAML::Node& key_node, const YAML::Node& cfg) {
        Id conn_id = key_node.as<Id>();
        Id sender_id = cfg["sender_id"].as<Id>();
        Id receiver_id = cfg["receiver_id"].as<Id>();
        std::uint64_t packets = cfg["number_of_packets"].as<std::uint64_t>();
        std::size_t packet_size = cfg["packet_size"].as<std::size_t>();

        std::string alg = cfg["mplb_algorithm"]
                              ? cfg["mplb_algorithm"].as<std::string>()
                              : "round_robin";

        auto policy = make_mplb_policy(alg);
        auto conn = std::make_shared<Connection>(conn_id, sender_id,
                                                 receiver_id, policy);
        conn->configure(packets, packet_size);

        return conn;
    }
};

}  // namespace sim