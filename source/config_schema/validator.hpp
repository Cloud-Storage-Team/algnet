#pragma once

#include <yaml-cpp/yaml.h>

#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim{

void validate(const YAML::Node& schema_node, const ConfigNodeWithPreset& config_node);

void validate_field(const YAML::Node& schema_node, const ConfigNodeWithPreset& config_node);

} // namespace sim