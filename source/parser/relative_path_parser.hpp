#pragma once

#include <string_view>

#include "parser/config_reader/config_node.hpp"

namespace sim {
inline std::filesystem::path parse_relative_path(const ConfigNode& node,
                                                 std::string_view field_name,
                                                 std::filesystem::path prefix) {
    return prefix /
           node[field_name].value_or_throw().as_or_throw<std::string>();
}
}  // namespace sim
