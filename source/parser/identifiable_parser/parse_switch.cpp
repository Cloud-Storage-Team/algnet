#include "device/ecn.hpp"
#include "device/switch.hpp"
#include "identifiable_parser.hpp"
#include "parser/parse_utils.hpp"

namespace sim {
static ECN parse_ecn(const YAML::Node& node) {
    float min = node["min"].as<float>();
    float max = node["max"].as<float>();
    float probability = node["probability"].as<float>();
    return ECN(min, max, probability);
}

template <>
std::shared_ptr<Switch> Parser<Switch>::parse_object(
    const YAML::Node& key_node, const YAML::Node& value_node) {
    Id id = key_node.as<Id>();
    std::unique_ptr<IHasher> hasher = nullptr;
    if (value_node["hasher"]) {
        hasher = parse_hasher(value_node);
    }
    const YAML::Node& ecn_node = value_node["ecn"];
    if (ecn_node) {
        return std::make_shared<Switch>(id, parse_ecn(ecn_node),
                                        std::move(hasher));
    }
    ECN default_ecn = ECN(1.0, 1.0, 0.0);
    return std::make_shared<Switch>(id, std::move(default_ecn),
                                    std::move(hasher));
}
}  // namespace sim
