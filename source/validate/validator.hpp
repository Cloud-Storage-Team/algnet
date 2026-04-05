#pragma once

#include <yaml-cpp/yaml.h>

#include "parser/config_reader/config_node_with_preset.hpp"


namespace sim{

void validate(const std::string& path, const ConfigNodeWithPreset& config_node);

bool is_meta_field(const std::string& field);

void validate_field(const YAML::Node schema_node, const ConfigNodeWithPreset& config_node);

} // namespace sim