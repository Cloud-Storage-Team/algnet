#include "device/switch.hpp"
#include "new_identifiable_parser.hpp"

namespace sim {
static ECN parse_ecn(const YAML::Node& node) {
    float min = node["min"].as<float>();
    float max = node["max"].as<float>();
    float probability = node["probability"].as<float>();
    return ECN(min, max, probability);
}

template <>
std::shared_ptr<Switch> IdentifieableParser::parse_object(
    const YAML::Node& key_node, const YAML::Node& value_node) {
    Id id = key_node.as<Id>();
    const YAML::Node& ecn_node = value_node["ecn"];
    if (ecn_node) {
        return std::make_shared<Switch>(id, parse_ecn(ecn_node));
    }
    return std::make_shared<Switch>(id);
}
}  // namespace sim