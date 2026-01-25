#include "./config_node_with_preset.hpp"

namespace sim{

ConfigNodeWithPreset::ConfigNodeWithPreset(ConfigNode a_node, ConfigNode a_presets_node)
    : m_node(std::move(a_node)), m_presets_node(std::move(a_presets_node)){}

ConfigNodeWithPreset::ConfigNodeWithPreset(ConfigNode a_node, std::optional<ConfigNode> a_preset, ConfigNode a_presets_node)
    : m_node(std::move(a_node)), m_preset(std::move(a_preset)), m_presets_node(std::move(a_presets_node)){}

ConfigNodeWithPresetExpected ConfigNodeWithPreset::operator[](std::string_view key) const{
    const ConfigNodeExpected child_node = m_node[key];
    if (!child_node.has_value()){
        if (!m_preset.has_value()){
            ConfigNodeExpected node_preset_name = m_node["preset-name"];
            if (!node_preset_name.has_value()){
                std::stringstream ss;
                ss << "Key error: node " << m_node.get_name().value() << ":\n";
                ss << "does not have key `preset-name`";
                return std::unexpected(ss.str());
            }
            std::string tmp = node_preset_name.value().get_node().as<std::string>();
            ConfigNodeExpected preset_node = m_presets_node[tmp];
            if (!preset_node.has_value()){
                std::stringstream ss;
                ss << "Key error: preset of node" << m_node.get_name().value() << ":\n";
                ss << "does not have key `" << key << '`';
                return std::unexpected(ss.str());
            }
            m_preset.emplace(preset_node.value());
            ConfigNodeExpected key_node = m_preset.value()[key];
            if (!key_node.has_value()){
                std::stringstream ss;
                ss << "Key error: preset node " << preset_node.value().get_name().value() << ":\n";
                ss << "does not have key `" << key << '`';
                return std::unexpected(ss.str());
            }
            return ConfigNodeWithPreset(key_node.value(), m_preset, m_presets_node);
        }
        ConfigNodeExpected key_node = m_preset.value()[key];
        if (!key_node.has_value()){
            std::stringstream ss;
            ss << "Key error: preset " << m_preset.value().get_name().value() << " ";
            ss << "does not have key: " << key;
            return std::unexpected(ss.str());
        }
        return ConfigNodeWithPreset(m_preset.value()[key].value(), m_preset, m_presets_node);
    }
    return ConfigNodeWithPreset(child_node.value(), m_preset, m_presets_node);
}

const ConfigNode& ConfigNodeWithPreset::get_node() const noexcept{
    return m_node;
}

const ConfigNode& ConfigNodeWithPreset::get_presets_node() const noexcept{
    return m_presets_node;
}

std::optional<ConfigNode>& ConfigNodeWithPreset::get_preset() const noexcept{
    return m_preset;
}

} // namespace sim