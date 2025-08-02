#include "object_parser.hpp"
#include "link/link.hpp"

namespace sim {

template <>
std::shared_ptr<ILink> Parser<ILink>::parse_object(const YAML::Node& key_node,
                                                 const YAML::Node& value_node) {
    return Parser<Link>::parse_object(key_node, value_node);
}
}  // namespace sim
