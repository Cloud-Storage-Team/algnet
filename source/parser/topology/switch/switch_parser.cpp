#include "switch_parser.hpp"

#include "device/hashers/adaptive_flowlet_hasher.hpp"
#include "device/hashers/ecmp_hasher.hpp"
#include "device/hashers/flowlet_hasher.hpp"
#include "device/hashers/random_hasher.hpp"
#include "device/hashers/salt_ecmp_hasher.hpp"
#include "device/hashers/symmetric_hasher.hpp"
#include "parser/parse_utils.hpp"
#include "parser/topology/ecn/ecn_parser.hpp"

namespace sim {

std::shared_ptr<ISwitch> SwitchParser::parse_i_switch(
    const ConfigNodeWithPreset& switch_node, const ConfigNodeWithPreset& packet_spraying_node) {
    return parse_default_switch(switch_node, packet_spraying_node);
}

std::shared_ptr<Switch> SwitchParser::parse_default_switch(
    const ConfigNodeWithPreset& switch_node, const ConfigNodeWithPreset& packet_spraying_node) {
    Id id = switch_node.get_node().get_name_or_throw();
    ConfigNodeWithPresetExpected ecn_node = switch_node["ecn"];
    ECN ecn(1.0, 1.0, 0.0);
    if (ecn_node.has_value()) {
        ecn = EcnParser::parse_ecn(ecn_node.value());
    }
    return std::make_shared<Switch>(id, std::move(ecn),
                                    parse_hasher(packet_spraying_node, id));
}

std::unique_ptr<IPacketHasher> SwitchParser::parse_hasher(
    const ConfigNodeWithPreset& packet_spraying_node, Id switch_id) {
    std::string type = packet_spraying_node["type"]
                           .value_or_throw()
                           .get_node()
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
            parse_time(packet_spraying_node["threshold"].value_or_throw().get_node());
        return std::make_unique<FLowletHasher>(threshold);
    }
    if (type == "adaptive_flowlet") {
        ConfigNodeWithPresetExpected factor_node = packet_spraying_node["factor"];
        if (factor_node.has_value()) {
            double factor = factor_node.value().get_node().as_or_throw<double>();
            return std::make_unique<AdaptiveFlowletHasher>(factor);
        } else {
            return std::make_unique<AdaptiveFlowletHasher>();
        }
    }
    if (type == "salt") {
        return std::make_unique<SaltECMPHasher>(std::move(switch_id));
    }
    throw packet_spraying_node.get_node().create_parsing_error(
        fmt::format("Unexpected packet sprayng type: {}", type));
}

}  // namespace sim
