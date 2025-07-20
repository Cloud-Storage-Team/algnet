#include <memory>

#include "device/host.hpp"
#include "identifiable_parser.hpp"
#include "parser/parse_utils.hpp"
#include "utils/hasher.hpp"

namespace sim {

template <>
std::shared_ptr<Host> Parser<Host>::parse_object(const YAML::Node& key_node,
                                                 const YAML::Node& value_node) {
    std::unique_ptr<IHasher> hasher = nullptr;
    if (value_node["hasher"]) {
        std::unique_ptr<IHasher> hasher = parse_hasher(value_node);
    }
    return std::make_shared<Host>(key_node.as<Id>(), std::move(hasher));
}

}  // namespace sim
