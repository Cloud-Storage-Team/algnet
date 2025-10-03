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
    PresetStorage(const YAML::Node& presets_node,
                  std::function<TPreset(const YAML::Node&)> preset_parser) {
        std::unordered_map<std::string, TPreset> presets;
        if (!presets_node) {
            return;
        }
        for (auto it = presets_node.begin(); it != presets_node.end(); ++it) {
            const YAML::Node key_node = it->first;
            const YAML::Node body_node = it->second;
            std::string preset_name = key_node.as<std::string>();

            if (this->contains(preset_name)) {
                throw std::runtime_error(fmt::format(
                    "Preset with name {} already exists", preset_name));
            }

            this->emplace(std::move(preset_name), preset_parser(body_node));
        }
    }

    // Gets preset with name node["preset-name"] from storage
    // If node does not contain "preset-name", use default
    // If there is no preset with such name, return default TPreset
    TPreset get_preset(const YAML::Node& node) const {
        static_assert(std::is_default_constructible_v<TPreset>,
                      "Preset must be default constructable");
        YAML::Node preset_name_node = node["preset-name"];

        if (preset_name_node) {
            // preset-name specified
            std::string preset_name = preset_name_node.as<std::string>();
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
