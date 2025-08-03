#include "parse_switch.hpp"

namespace sim {
    
std::shared_ptr<ISwitch> ParseSwitch::parse_i_switch(const YAML::Node& key_node,
                                                 const YAML::Node& value_node) {
    return parse_default_switch(key_node, value_node);
}

std::shared_ptr<Switch> ParseSwitch::parse_default_switch(const YAML::Node& key_node,
                                            [[maybe_unused]] const YAML::Node& value_node) {
    Id id = key_node.as<Id>();
    const YAML::Node& ecn_node = value_node["ecn"];
    if (ecn_node) {
        return std::make_shared<Switch>(id, parse_ecn(ecn_node));
    }
    return std::make_shared<Switch>(id);                                     
}

ECN ParseSwitch::parse_ecn(const YAML::Node& node) {
    float min = node["min"].as<float>();
    float max = node["max"].as<float>();
    float probability = node["probability"].as<float>();
    return ECN(min, max, probability);
}

}  // namespace sim
