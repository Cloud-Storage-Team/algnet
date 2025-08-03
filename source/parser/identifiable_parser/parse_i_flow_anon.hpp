#pragma once
#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

#include <memory>

#include "flow/i_flow.hpp"

namespace sim {
    
std::shared_ptr<IFlow> parse_i_flow(const YAML::Node& key_node,
                                                 const YAML::Node& value_node);
}  // namespace sim
