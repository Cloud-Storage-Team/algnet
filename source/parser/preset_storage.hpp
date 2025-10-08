#pragma once
#include <functional>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "parse_utils.hpp"
#include "parser/config_reader/config_node.hpp"
#include "utils/errors/base_error.hpp"

namespace sim {

template <typename TPreset>
class PresetStorage : public std::unordered_map<std::string, TPreset> {
public:
    PresetStorage() = default;

    static PresetStorage<TPreset> parse_presets(
        const ConfigNode& presets_node,
        std::function<TPreset(const ConfigNode&)> preset_parser) {
        PresetStorage<TPreset> storage;
        for (auto it = presets_node.begin(); it != presets_node.end(); ++it) {
            ConfigNode preset_node = *it;
            std::string preset_name = preset_node.get_name().value();

            if (storage.contains(preset_name)) {
                std::stringstream ss;
                ss << "Error while parsing\n";
                ss << preset_node.get_stacktrace()->to_string() << "\n";
                ss << "Preset with name" << preset_name << "already exists";

                throw ConfigNodeError(ss.str());
            }

            storage.emplace(std::move(preset_name), preset_parser(preset_node));
        }
        return storage;
    }

    // Gets preset with name node["preset-name"] from storage
    // If node does not contain "preset-name", use default
    // If there is no preset with such name, return default TPreset
    TPreset get_preset(const ConfigNode& node) const {
        static_assert(std::is_default_constructible_v<TPreset>,
                      "Preset must be default constructable");
        ConfigNodeExpected preset_name_node = node["preset-name"];

        if (preset_name_node) {
            // preset-name specified
            std::string preset_name =
                preset_name_node.value().as<std::string>().value_or_throw();
            auto it = this->find(preset_name);
            if (it != this->end()) {
                return it->second;
            }
            throw utils::BaseError(
                fmt::format("Can not find preset with name {}", preset_name));
        } else {
            // Use default preset
            auto it = this->find(M_DEFAULT_PRESET_NAME);
            if (it != this->end()) {
                return it->second;
            }
            LOG_WARN(
                "Can not find default preset; use empty preset (get from "
                "default constructor)");
            return TPreset();
        }
    };

private:
    static inline std::string M_DEFAULT_PRESET_NAME = "default";
};

}  // namespace sim
