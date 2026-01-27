#include "./config_node_with_preset.hpp"

namespace sim{

ConfigNodeWithPreset::ConfigNodeWithPreset(ConfigNode a_node, std::optional<ConfigNode> a_presets_node, std::optional<ConfigNode> a_preset)
    : m_node(std::move(a_node)), m_presets_node(std::move(a_presets_node)), m_preset(std::move(a_preset)){}

ConfigNodeWithPresetExpected ConfigNodeWithPreset::operator[](std::string_view key) const{
    const ConfigNodeExpected child_node = m_node[key];
    if (!child_node.has_value()){
        if (!m_preset.has_value()){
            if (!m_presets_node.has_value()){
                std::stringstream ss;
                ss << "Key error: node " << m_node << ":\n";
                ss << "does not have key " << key;
                return std::unexpected(ss.str());
            }
            ConfigNodeExpected node_preset_name = m_node["preset-name"];
            if (!node_preset_name.has_value()){
                std::stringstream ss;
                ss << "Key error: node " << m_node << ":\n";
                ss << "does not have key `preset-name`";
                return std::unexpected(ss.str());
            }
            ConfigNode presets_node = m_presets_node.value();
            std::string preset_name = node_preset_name.value().as_or_throw<std::string>();
            ConfigNodeExpected preset_node = presets_node[preset_name];
            if (!preset_node.has_value()){
                std::stringstream ss;
                ss << "Error: preset named" << preset_name << "of node" << m_node << '\n';
                ss << "can not be found in presets node:" << presets_node << ":\n";
                return std::unexpected(ss.str());
            }
            m_preset.emplace(preset_node.value());
        }
        ConfigNodeExpected key_node = m_preset.value()[key];
        if (!key_node.has_value()){
            std::stringstream ss;
            ss << "Key error: preset " << m_preset.value().get_name().value() << " ";
            ss << "does not have key: " << key;
            return std::unexpected(ss.str());
        }
        return ConfigNodeWithPreset(key_node.value(), m_presets_node, m_preset);
    }
    return ConfigNodeWithPreset(child_node.value(), m_presets_node, m_preset);
}

const ConfigNode& ConfigNodeWithPreset::get_node() const noexcept{
    return m_node;
}

} // namespace sim