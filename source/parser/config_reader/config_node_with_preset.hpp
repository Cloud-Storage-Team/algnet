#pragma once

#include "./config_node.hpp"

namespace sim{

class ConfigNodeWithPreset;

using ConfigNodeWithPresetExpected = utils::StrExpected<ConfigNodeWithPreset>;

class ConfigNodeWithPreset{
public:
    ConfigNodeWithPreset(ConfigNode a_node, ConfigNode a_presets_node);

    ConfigNodeWithPreset(ConfigNode a_node, std::optional<ConfigNode> a_preset, ConfigNode a_presets_node);

    ConfigNodeWithPresetExpected operator[](std::string_view key) const;

    const ConfigNode& get_node() const noexcept;

    std::optional<ConfigNode>& get_preset() const noexcept;

    const ConfigNode& get_presets_node() const noexcept;

    template <typename T>
    [[nodiscard]] utils::StrExpected<T> as() const noexcept {
        try {
            return m_node.as<T>();
        } catch (const std::exception& e) {
            return std::unexpected(e.what());
        } catch (...) {
            return std::unexpected(
                "Undefined exception while calling `YAML::Node::as`");
        }
    }

private:

    // m_node - contains information about config node and preset name
    // m_preset - preset node which is used in m_node
    // m_presets_node - set of presets
    const ConfigNode m_node;
    mutable std::optional<ConfigNode> m_preset;
    const ConfigNode m_presets_node;
};

} // namespace sim 