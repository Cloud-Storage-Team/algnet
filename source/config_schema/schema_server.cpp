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
            ss << "Unknown field '" << field;
            ss << "' found in configuration:\n";
            ss << subnode;
            ss << "This field is not described in schema:\n";
            ss << schema_node;
            throw config_node.create_parsing_error(ss.str());
        }
    }

    // check if config node has all required fields
    for (const auto& required_field : schema_fields) {
        ConfigNodeWithPresetExpected exp_field_config =
            config_node[required_field];
        if (!exp_field_config.has_value()) {
            std::stringstream ss;
            ss << "Required field '" << required_field;
            ss << "' is missing in configuration: \n";
            ss << config_node << '\n';
            ss << "Field is required by schema node: \n";
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
    std::string type = schema_node["_type"].value().as<std::string>().value();
    if (type == "size_t") {
        config_node.as_or_throw<size_t>();
    } else if (type == "int") {
        config_node.as_or_throw<int>();
    } else if (type == "double") {
        config_node.as_or_throw<double>();
    } else if (type == "bool") {
        config_node.as_or_throw<bool>();
    } else if (type == "string") {
        config_node.as_or_throw<std::string>();
    } else if (type == "regex") {
        std::string pattern = config_node.as_or_throw<std::string>();
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
    ConfigSchema type_node = schema_node["_type"].value();
    std::string type = type_node.as_or_throw<std::string>();
    if (type.ends_with(".schema")) {
        std::filesystem::path nested_schema_path = std::filesystem::path(type);
        std::filesystem::path sub_schema_path =
            nested_schema_path.is_absolute()
                ? m_schemas_dir / nested_schema_path.relative_path()
                : std::filesystem::path(__FILE__).parent_path() /
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
        ss << "Unknown specified type '" << type << "' in schema: ";
        ss << schema_node;
        throw schema_node.create_parsing_error(ss.str());
    } else {
        if (!config_node.IsMap()) {
            std::stringstream ss;
            ss << "Expected object/map for config node:\n";
            ss << config_node << ".\n";
            ss << "Because schema\n";
            ss << schema_node << '\n';
            ss << " has nested fields";
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