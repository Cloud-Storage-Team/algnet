#include "packet_hasher_parser.hpp"

#include "parser/parse_utils.hpp"
#include "topology/device/hashers/adaptive_flowlet_hasher.hpp"
#include "topology/device/hashers/ecmp_hasher.hpp"
#include "topology/device/hashers/flowlet_hasher.hpp"
#include "topology/device/hashers/random_hasher.hpp"
#include "topology/device/hashers/salt_ecmp_hasher.hpp"
#include "topology/device/hashers/symmetric_hasher.hpp"

namespace sim {

std::unique_ptr<IPacketHasher> parse_packet_hasher(
    const ConfigNode& packet_spraying_node, const Id& switch_id) {
    std::string type = packet_spraying_node["type"]
                           .value_or_throw()
                           .as<std::string>()
                           .value_or_throw();
    if (type == "random") {
        return std::make_unique<RandomHasher>();
    }
    if (type == "ecmp") {
        return std::make_unique<ECMPHasher>();
    }
    if (type == "flowlet") {
        TimeNs threshold =
            parse_time(packet_spraying_node["threshold"].value_or_throw());
        return std::make_unique<FLowletHasher>(threshold);
    }
    if (type == "adaptive_flowlet") {
        ConfigNodeExpected factor_node = packet_spraying_node["factor"];
        if (factor_node.has_value()) {
            double factor = factor_node.value().as_or_throw<double>();
            return std::make_unique<AdaptiveFlowletHasher>(factor);
        } else {
            return std::make_unique<AdaptiveFlowletHasher>();
        }
    }
    if (type == "salt") {
        return std::make_unique<SaltECMPHasher>(std::move(switch_id));
    }
    throw packet_spraying_node.create_parsing_error(
        fmt::format("Unexpected packet sprayng type: {}", type));
}

}  // namespace sim