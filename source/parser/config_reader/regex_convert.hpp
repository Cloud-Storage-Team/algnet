#pragma once
#include <yaml-cpp/yaml.h>

#include <regex>

namespace YAML {
template <>
struct convert<std::regex> {
    static bool decode(const Node& node, std::regex& re) {
        if (!node.IsScalar()) {
            return false;
        }

        try {
            re = std::regex(node.as<std::string>());
        } catch (const std::regex_error& e) {
            return false;
        }
        return true;
    }
};
}  // namespace YAML