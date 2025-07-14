#pragma once
#include <yaml-cpp/yaml.h>

#include <memory>

#include "utils/identifier_factory.hpp"

namespace sim {

class IdentifieableParser {
public:
    template <typename T>
    static std::shared_ptr<T> parse_and_registrate(
        const YAML::Node& key_node, const YAML::Node& value_node) {
        std::shared_ptr<T> object = parse_object<T>(key_node, value_node);
        if (!IdentifierFactory::get_instance().add_object(object)) {
            throw std::runtime_error("Can not add object");
        }
        return object;
    }

private:
    // Parses object and return shared_ptr to it
    template <typename T>
    static std::shared_ptr<T> parse_object(const YAML::Node& key_node,
                                           const YAML::Node& value_node);
};

}  // namespace sim