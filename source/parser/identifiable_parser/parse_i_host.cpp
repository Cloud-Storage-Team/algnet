#include "device/host.hpp"
#include "object_parser.hpp"

namespace sim {

template <>
std::shared_ptr<IHost> Parser<IHost>::parse_object(
    const YAML::Node& key_node, const YAML::Node& value_node) {
    return Parser<Host>::parse_object(key_node, value_node);
}

}  // namespace sim