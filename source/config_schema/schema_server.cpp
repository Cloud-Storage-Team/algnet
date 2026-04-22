#include "schema_server.hpp"

#include <spdlog/fmt/fmt.h>

#include <regex>
#include <unordered_set>

namespace sim {

SchemaServer::SchemaServer(const std::filesystem::path& a_schemas_dir)
    : m_schemas_dir(std::move(a_schemas_dir)) {}

bool SchemaServer::is_meta_field(const std::string& field) {
    return field.starts_with('_');
}

void SchemaServer::validate_untyped(const ConfigSchema& schema_node,
                                    const ConfigNodeWithPreset& config_node) {
    // create set of requried fields
    std::unordered_set<std::string> schema_fields;
    for (const auto& it : schema_node) {
        std::optional<std::string> tmp_field = it.get_name();
        if (!tmp_field) {
            std::stringstream ss;
            ss << "Empty schema node:\n";
            ss << it;
            throw schema_node.create_parsing_error(ss.str());
        }
        std::string field = tmp_field.value();
        if (!is_meta_field(field)) {
            schema_fields.insert(field);
        }
    }

    // check if config node has unknown fields
    for (const auto& node : config_node) {
        std::optional<std::string> tmp_field = node.get_name();
        if (!tmp_field) {
            std::stringstream ss;
            ss << "Empty config node:\n";
            ss << node;
            throw schema_node.create_parsing_error(ss.str());
        }
        std::string field = tmp_field.value();
        if (!schema_fields.contains(field)) {
            std::stringstream ss;
            ss << "Field does not correspond to schema:\n";
            ss << schema_node << '\n';
            throw config_node.create_parsing_error(ss.str());
        }
    }

    // check if config node has all required fields
    for (const auto& it : schema_node) {
        std::string field_schema = it.get_name().value();
        if (is_meta_field(field_schema)) {
            continue;
        }
        ConfigNodeWithPresetExpected field_config = config_node[field_schema];
        if (!field_config.has_value()) {
            std::stringstream ss;
            ss << "Does not have required field '"
               << field_config.get_name().value_or("null")
               << "' described in schema:\n";
            ss << schema_node;
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
            throw std::runtime_error("Incorrect type specified std::regex");
        }
    } else {
        return false;
    }
    return true;
}

[[nodiscard]] bool SchemaServer::try_validate_custom_types(
    const ConfigSchema& schema_node, const ConfigNodeWithPreset& config_node) {
    std::string type = schema_node["_type"].value().as<std::string>().value();
    if (type.ends_with(".schema")) {
        ConfigSchema sub_schema = load_file(m_schemas_dir / type);
        validate(sub_schema, config_node);
        return true;
    } else {
        return false;
    }
}

void SchemaServer::validate(const ConfigSchema& schema_node,
                            const ConfigNodeWithPreset& config_node) {
    if (schema_node["_type"]) {
        utils::StrExpected<std::string> check_type =
            schema_node["_type"].value().as<std::string>();
        if (!check_type.has_value()) {
            std::stringstream ss;
            ss << "Schema: " << schema_node << '\n';
            ss << "has empty field '_type'";
            throw schema_node.create_parsing_error(ss.str());
        }
        std::string type = check_type.value();
        if (try_validate_basic_types(schema_node, config_node)) {
            return;
        }
        if (try_validate_custom_types(schema_node, config_node)) {
            return;
        }
        throw schema_node.create_parsing_error(
            fmt::format("Unknown type: {}", type));
    } else {
        if (!config_node.IsMap()) {
            throw std::runtime_error(config_node.get_name_or_throw() +
                                     " must be object.");
        }
        validate_untyped(schema_node, config_node);
    }
}

void SchemaServer::validate(const std::filesystem::path& schema_path,
                            const ConfigNodeWithPreset& config_node) {
    std::filesystem::path full_path;
    if (schema_path.is_absolute()) {
        full_path = schema_path;
    } else {
        full_path = m_schemas_dir / schema_path;
    }
    ConfigSchema schema_node = load_file(schema_path);
    validate(schema_node, config_node);
}

}  // namespace sim