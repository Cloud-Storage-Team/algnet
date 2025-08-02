#include "device/host.hpp"
#include "parser/identifiable_parser/identifiable_parser.hpp"

namespace sim {

template <>
std::shared_ptr<Host> Parser<Host>::parse_object(
    const YAML::Node& key_node, const YAML::Node& value_node) {
    (void)value_node;
    return std::make_shared<Host>(key_node.as<Id>());
}

}  // namespace sim