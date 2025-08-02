#include "device/switch.hpp"
#include "object_parser.hpp"

namespace sim {

template <>
std::shared_ptr<ISwitch> Parser<ISwitch>::parse_object(
    const YAML::Node& key_node, const YAML::Node& value_node) {
    return Parser<Switch>::parse_object(key_node, value_node);
}

}  // namespace sim
