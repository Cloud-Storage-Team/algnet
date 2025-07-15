#pragma once
#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

#include <memory>
#include <type_traits>

#include "utils/identifier_factory.hpp"

namespace sim {

class IdentifieableParser {
public:
    template <typename T>
    static std::shared_ptr<T> parse_and_registrate(
        const YAML::Node& key_node, const YAML::Node& value_node) {
        static_assert(std::is_base_of_v<Identifiable, T>);
        std::shared_ptr<T> object = parse_object<T>(key_node, value_node);
        if (!IdentifierFactory::get_instance().add_object(object)) {
            throw std::runtime_error(fmt::format(
                "Can not add object with type {}; object with same id ({}) "
                "already exists",
                typeid(T).name(), object.get()->get_id()));
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