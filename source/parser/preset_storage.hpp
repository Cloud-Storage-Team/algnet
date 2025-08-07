#pragma once
#include <yaml-cpp/yaml.h>

#include <functional>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "parse_utils.hpp"

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
                    "Preset with name {} alredy exists", preset_name));
            }

            this->emplace(std::move(preset_name), preset_parser(body_node));
        }
    }

    // Gets preset with name node["preset-name"] from storage
    // If node does noit contain "preset-name", use default
    // If there is no preser with such name, return default TPreset
    TPreset get_preset(const YAML::Node& node,
                       std::optional<Id> object_id = std::nullopt) const {
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
            std::string error_message =
                fmt::format("Can not find preset with name {}", preset_name);
            if (object_id.has_value()) {
                error_message +=
                    fmt::format("  for object {}", object_id.value());
            }

            throw std::runtime_error(std::move(error_message));
        } else {
            // Use default preset
            auto it = this->find(M_DEFAULT_PRESET_NAME);
            if (it != this->end()) {
                return it->second;
            }
            std::string warn_message =
                "Can not find default preset; use empty preset (get from "
                "default constructor)";
            if (object_id.has_value()) {
                warn_message +=
                    fmt::format(" for object {}", object_id.value());
            }
            LOG_WARN(std::move(warn_message));
            return TPreset();
        }
    };

private:
    const static inline std::string M_DEFAULT_PRESET_NAME = "default";
};

}  // namespace sim
