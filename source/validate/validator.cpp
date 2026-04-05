#include "validator.hpp"

#include <regex>
#include <unordered_set>

namespace sim{

void validate(const YAML::Node& schema_node, const ConfigNodeWithPreset& config_node){
    // create set of requried fields
    std::unordered_set<std::string> schema_fields;
    for (auto it: schema_node){
        std::string field = it.first.as<std::string>();
        if (!is_meta_field(field)){
            schema_fields.insert(field);
        }
    }

    // check if config node has unknown fields
    for (const auto& node: config_node){
        std::string field = node.get_name_or_throw();
        if (!schema_fields.contains(field)){
            std::stringstream ss;
            ss << "Config node: " << config_node.get_name_or_throw();
            ss << "has unknown field: " << field;
            throw std::runtime_error(ss.str());
        }
    }

    // check if config node has all required fields
    for (auto it: schema_node){
        std::string field_schema = it.first.as<std::string>();
        if (is_meta_field(field_schema)){
            continue;
        }
        ConfigNodeWithPresetExpected field_config = config_node[field_schema];
        if (!field_config.has_value()){
            std::stringstream ss;
            ss << "Config node: " << config_node.get_name_or_throw();
            ss << "hasn't required field: " << field_schema;
            throw std::runtime_error(ss.str());
        }
    }

    for (auto it: schema_node){
        std::string field = it.first.as<std::string>();
        if (!is_meta_field(field)){
            validate_field(it.second, config_node[field].value());
        }
    }
}

bool is_meta_field(const std::string& field){
    return !field.empty() && field[0] == '_';
}

void validate_field(const YAML::Node schema_node, const ConfigNodeWithPreset& config_node){
    if (schema_node["_type"]){
        std::string type = schema_node["_type"].as<std::string>();
        if (type == "size_t"){
            config_node.as_or_throw<size_t>();
        } else if (type == "int"){
            config_node.as_or_throw<int>();
        } else if (type == "double"){
            config_node.as_or_throw<double>();
        } else if (type == "bool"){
            config_node.as_or_throw<bool>();
        } else if (type == "string"){
            config_node.as_or_throw<std::string>();
        } else if (type == "regex"){
            std::string pattern = config_node.as_or_throw<std::string>();
            try {
                std::regex r(pattern);
            } catch (const std::regex_error&) {
                throw std::runtime_error("Incorrect type specified std::regex");
            }
        } else if (type.ends_with(".schema")){
            YAML::Node sub_schema = YAML::LoadFile(type);
            if (!config_node.IsMap()){
                throw std::runtime_error(config_node.get_name_or_throw() + " must be object.");
            }
            validate(sub_schema, config_node);
        } else{
            throw std::runtime_error("Unknown type: " + type);
        }
    } else{
        if (!config_node.IsMap()){
            throw std::runtime_error(config_node.get_name_or_throw() + " must be object.");
        }
        validate(schema_node, config_node);
    }
}

} //namespace sim