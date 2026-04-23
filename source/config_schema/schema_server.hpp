#pragma once

#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {

using ConfigSchema = ConfigNode;

class SchemaServer {
public:
    SchemaServer(const std::filesystem::path& a_schema_path);

    void validate(const ConfigSchema& schema_node,
                  const ConfigNodeWithPreset& config_node);

    void validate(const std::filesystem::path& schema_path,
                  const ConfigNodeWithPreset& config_node);

private:
    // if schema_node correspond to basic type, validate config_node and returns
    // true otherwise, returns false
    [[nodiscard]] bool try_validate_basic_types(
        const ConfigSchema& schema_node,
        const ConfigNodeWithPreset& config_node);

    // if schema_node correspond to custom type, validate config_node and
    // returns true otherwise, returns false
    [[nodiscard]] bool try_validate_custom_types(
        const ConfigSchema& schema_node,
        const ConfigNodeWithPreset& config_node);

    void validate_untyped(const ConfigSchema& schema_node,
                          const ConfigNodeWithPreset& config_node);

    static bool is_meta_field(const std::string& field);

private:
    const std::filesystem::path m_schemas_dir;
};

}  // namespace sim