#pragma once
#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

#include <memory>
#include <type_traits>

namespace sim {

template <typename T>
class Parser  {
public:
    // Parses object and return shared_ptr to it
    static std::shared_ptr<T> parse_object(const YAML::Node& key_node,
                                           const YAML::Node& value_node);
};


}  // namespace sim