#include "switch_parser.hpp"

#include "parser/parse_utils.hpp"
#include "utils/errors/base_error.hpp"
#include "utils/errors/error_with_id.hpp"

namespace sim {

std::shared_ptr<ISwitch> SwitchParser::parse_i_switch(
    const YAML::Node& key_node, const YAML::Node& value_node,
    const YAML::Node& packet_spraying_node, const SwitchPresets& presets) {
    return parse_default_switch(key_node, value_node, packet_spraying_node, presets);
}

void SwitchParser::parse_to_args(const YAML::Node& node, SwitchInitArgs& args) {
    ECN ecn(1.0, 1.0, 0.0);

    auto ecn_node = node["ecn"];
    if (ecn_node) {
        ecn = parse_ecn(ecn_node);
    }
    args.ecn.emplace(ecn);
}

std::shared_ptr<Switch> SwitchParser::parse_default_switch(
    const YAML::Node& key_node, const YAML::Node& value_node,
    const YAML::Node& packet_spraying_node, const SwitchPresets& presets) {
    Id id = key_node.as<Id>();
    try {
        SwitchInitArgs switch_args = presets.get_preset(value_node);
        switch_args.id = id;
        parse_to_args(value_node, switch_args);
        switch_args.hasher = std::move(parse_hasher(packet_spraying_node, id));
        return std::make_shared<Switch>(std::move(switch_args));
    } catch (const utils::BaseError& base_error) {
        throw utils::ErrorWithId(base_error, id);
    } catch (...) {
        throw;
    }
    const YAML::Node& ecn_node = value_node["ecn"];
    ECN ecn(1.0, 1.0, 0.0);
    if (ecn_node) {
        ecn = parse_ecn(ecn_node);
    }
    return std::make_shared<Switch>(id, std::move(ecn),
                                    parse_hasher(packet_spraying_node, id));
}

ECN SwitchParser::parse_ecn(const YAML::Node& node) {
    float min = node["min"].as<float>();
    float max = node["max"].as<float>();
    float probability = node["probability"].as<float>();
    return ECN(min, max, probability);
}

std::unique_ptr<IPacketHasher> SwitchParser::parse_hasher(
    const YAML::Node& packet_spraying_node, Id switch_id) {
    auto type_node = packet_spraying_node["type"];
    std::string type = type_node.as<std::string>();
    if (type == "random") {
        return std::make_unique<RandomHasher>();
    }
    if (type == "ecmp") {
        return std::make_unique<ECMPHasher>();
    }
    if (type == "flowlet") {
        TimeNs threshold =
            parse_time(packet_spraying_node["threshold"].as<std::string>());
        return std::make_unique<FLowletHasher>(threshold);
    }
    if (type == "salt") {
        return std::make_unique<SaltECMPHasher>(std::move(switch_id));
    }
    throw std::runtime_error(fmt::format("Unexpected packet sprayng type: {}", type));
}

}  // namespace sim
