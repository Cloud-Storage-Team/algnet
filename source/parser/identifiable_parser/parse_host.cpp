#include <memory>

#include "device/host.hpp"
#include "identifiable_parser.hpp"
#include "utils/hasher.hpp"

namespace sim {

template <>
std::shared_ptr<Host> Parser<Host>::parse_object(const YAML::Node& key_node,
                                                 const YAML::Node&) {
    return std::make_shared<Host>(key_node.as<Id>());
}

template <>
template <>
std::shared_ptr<Host> Parser<Host>::parse_object(
    const YAML::Node& key_node, const YAML::Node&,
    std::unique_ptr<IHasher>&& hasher) {
    return std::make_shared<Host>(key_node.as<Id>(), std::move(hasher));
}

}  // namespace sim
