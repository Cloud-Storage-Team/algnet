#pragma once
#include <yaml-cpp/yaml.h>

#include <functional>
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
    TPreset get_preset(const YAML::Node& node) const {
        static_assert(std::is_default_constructible_v<TPreset>,
                      "Preset must be default constructable");
        std::string preset_name = parse_with_default<std::string>(
            node, "preset-name", std::identity{}, "default");

        // Get args from preset if it exists
        auto it = this->find(preset_name);
        if (it != this->end()) {
            return it->second;
        }

        return TPreset();
    };
};

}  // namespace sim
