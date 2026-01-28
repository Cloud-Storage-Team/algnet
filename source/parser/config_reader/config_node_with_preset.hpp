#pragma once

#include "./config_node.hpp"

namespace sim{

class ConfigNodeWithPreset;

using ConfigNodeWithPresetExpected = utils::StrExpected<ConfigNodeWithPreset>;

// Config node that supports preset inheritance.
class ConfigNodeWithPreset{
public:

    explicit ConfigNodeWithPreset(
        ConfigNode a_node,
        std::optional<ConfigNode> a_presets_node = std::nullopt,
        std::optional<ConfigNode> a_preset = std::nullopt
    );

    ConfigNodeWithPresetExpected operator[](std::string_view key) const;

    // Returns the base config node.
    const ConfigNode& get_node() const noexcept;

    // Converts m_node to type T. Probably this method will return message about error
    template <typename T>
    [[nodiscard]] utils::StrExpected<T> as() const noexcept {
        return m_node.as<T>();
    }

private:

    // m_node contains information about config node and probably preset name
    // m_preset - preset node which that is used to supplement fields of m_node
    // m_presets_node - node it which preset node should be searched (id it needs)
    const ConfigNode m_node;
    const std::optional<ConfigNode> m_presets_node;
    mutable std::optional<ConfigNode> m_preset;
};

} // namespace sim 