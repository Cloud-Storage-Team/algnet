#include "schema_server.hpp"

#include <spdlog/fmt/fmt.h>

#include <regex>
#include <unordered_set>

namespace sim {

SchemaServer::SchemaServer(const std::filesystem::path& a_schemas_dir)
    : m_schemas_dir(a_schemas_dir) {}

bool SchemaServer::is_meta_field(const std::string& field) {
    return field.starts_with('_');
}

void SchemaServer::validate_untyped(const ConfigSchema& schema_node,
                                    const ConfigNodeWithPreset& config_node) {
    // create set of requried fields
    std::unordered_set<std::string> schema_fields;
    for (const auto& it : schema_node) {
        const std::string& field = it.get_name_or_throw();
        if (!is_meta_field(field)) {
            schema_fields.insert(field);
        }
    }

    // check if config node has unknown fields
    for (const auto& subnode : config_node) {
        const std::string& field = subnode.get_name_or_throw();
        if (!schema_fields.contains(field)) {
            std::stringstream ss;
            ss << "Node has field `'" << field
               << "' that does not described in schema:\n";
            ss << schema_node;
            throw subnode.create_parsing_error(ss.str());
        }
    }

    // check if config node has all required fields
    for (const auto& required_field : schema_fields) {
        ConfigNodeWithPresetExpected exp_field_config =
            config_node[required_field];
        if (!exp_field_config.has_value()) {
            std::stringstream ss;
            ss << "Missing required field '" << required_field
               << "' described in schema: \n";
            ss << schema_node << "\n";
            throw config_node.create_parsing_error(ss.str());
        }
    }

    for (const auto& it : schema_node) {
        std::string field = it.get_name().value();
        if (!is_meta_field(field)) {
            validate(schema_node[field].value(), config_node[field].value());
        }
    }
}

[[nodiscard]] bool SchemaServer::try_validate_basic_types(
    const ConfigSchema& schema_node, const ConfigNodeWithPreset& config_node) {
    const ConfigSchema& type_node = schema_node["_type"].value();
    std::string type = type_node.as_or_throw<std::string>();
    auto unsafe_cast_config_node_to = [&]<typename T>() -> T {
        auto as_result = config_node.as<T>();
        if (!as_result.has_value()) {
            std::stringstream ss;
            ss << "Node should contain basic type `" << type
               << "' due to schema:\n";
            ss << type_node << "\n";
            ss << "But its not:\n";
            ss << as_result.error() << '\n';
            throw config_node.create_parsing_error(ss.str());
        }
        return as_result.value();
    };
    if (type == "size_t") {
        unsafe_cast_config_node_to.operator()<size_t>();
    } else if (type == "int") {
        unsafe_cast_config_node_to.operator()<int>();
    } else if (type == "double") {
        unsafe_cast_config_node_to.operator()<double>();
    } else if (type == "bool") {
        unsafe_cast_config_node_to.operator()<bool>();
    } else if (type == "string") {
        unsafe_cast_config_node_to.operator()<std::string>();
    } else if (type == "regex") {
        std::string pattern =
            unsafe_cast_config_node_to.operator()<std::string>();
        try {
            std::regex r(pattern);
        } catch (const std::regex_error&) {
            std::stringstream ss;
            ss << "Field must contain valid regular expression.\n";
            ss << "Regex pattern: " << pattern << '\n';
            throw config_node.create_parsing_error(ss.str());
        }
    } else {
        return false;
    }
    return true;
}

[[nodiscard]] bool SchemaServer::try_validate_custom_types(
    const ConfigSchema& schema_node, const ConfigNodeWithPreset& config_node) {
    const ConfigSchema& type_node = schema_node["_type"].value();
    std::string type = type_node.as_or_throw<std::string>();
    if (type.ends_with(".schema")) {
        std::filesystem::path nested_schema_path = std::filesystem::path(type);
        std::filesystem::path sub_schema_path =
            nested_schema_path.is_absolute()
                ? m_schemas_dir / nested_schema_path.relative_path()
                : std::filesystem::path(schema_node.get_config_path().value())
                          .parent_path() /
                      nested_schema_path;
        validate(sub_schema_path, config_node);
        return true;
    } else {
        return false;
    }
}

void SchemaServer::validate(const ConfigSchema& schema_node,
                            const ConfigNodeWithPreset& config_node) {
    ConfigNodeExpected exp_type_node = schema_node["_type"];
    if (exp_type_node) {
        std::string type = exp_type_node.value().as_or_throw<std::string>();
        if (try_validate_basic_types(schema_node, config_node)) {
            return;
        }
        if (try_validate_custom_types(schema_node, config_node)) {
            return;
        }
        std::stringstream ss;
        throw schema_node.create_parsing_error(
            fmt::format("Unknown specified type '{}'", type));
    } else {
        if (!config_node.IsMap()) {
            std::stringstream ss;
            ss << "Should be map due to schema\n";
            ss << schema_node << '\n';
            throw config_node.create_parsing_error(ss.str());
        }
        validate_untyped(schema_node, config_node);
    }
}

void SchemaServer::validate(const std::filesystem::path& schema_path,
                            const ConfigNodeWithPreset& config_node) {
    std::filesystem::path full_path =
        schema_path.is_absolute() ? schema_path : m_schemas_dir / schema_path;
    auto exp_sub_schema = safe_load_file(full_path);
    if (!exp_sub_schema) {
        throw config_node.create_parsing_error(
            fmt::format("Failed to parse corresponding schema file: {}",
                        exp_sub_schema.error()));
    }
    validate(exp_sub_schema.value(), config_node);
}

}  // namespace sim