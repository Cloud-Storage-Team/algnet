#pragma once
#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

#include <memory>
#include <type_traits>

#include "utils/identifier_factory.hpp"

namespace sim {

template <typename T>
class Parser  {
public:
    // Parses object and return shared_ptr to it
    static std::shared_ptr<T> parse_object(const YAML::Node& key_node,
                                           const YAML::Node& value_node);
};

template <typename T>
class IdentifieableParser {
    static_assert(std::is_base_of_v<Identifiable, T>);

public:
    static std::shared_ptr<T> parse_and_registrate(
        const YAML::Node& key_node, const YAML::Node& value_node) {
        std::shared_ptr<T> object = Parser<T>::parse_object(key_node, value_node);
        registrate(object);
        return object;
    }

private:
    static void registrate(std::shared_ptr<T> object) {
        if (!IdentifierFactory::get_instance().add_object(object)) {
            throw std::runtime_error(fmt::format(
                "Can not add object with type {}; object with same id ({}) "
                "already exists",
                typeid(T).name(), object.get()->get_id()));
        }
        return object;
    }
};

}  // namespace sim