#include "./config_node_with_preset.hpp"

namespace sim {

ConfigNodeWithPreset::ConfigNodeWithPreset(
    ConfigNode a_node, std::optional<ConfigNode> a_presets_node,
    std::optional<ConfigNode> a_preset)
    : m_node(std::move(a_node)),
      m_presets_node(std::move(a_presets_node)),
      m_preset(std::move(a_preset)) {}

ConfigNodeWithPresetExpected ConfigNodeWithPreset::operator[](
    std::string_view key) const {
    const ConfigNodeExpected child_node = m_node[key];
    if (!child_node.has_value()) {
        // m_node hasn't key. Tries to find key in m_preset
        if (!m_preset.has_value()) {
            // m_preset doesn't exist. Tries to find preset for this node
            if (!m_presets_node.has_value()) {
                // m_presets_node doesn't exist. Returns message about it
                std::stringstream ss;
                ss << "Key error: node " << m_node << ":\n";
                ss << "does not have key " << key;
                return std::unexpected(ss.str());
            }
            // m_presets_node exist. Tries to find field 'preset-name' in m_node
            ConfigNodeExpected node_preset_name = m_node["preset-name"];
            if (!node_preset_name.has_value()) {
                // config hasn't field 'preset-name'. Returns message about it
                std::stringstream ss;
                ss << "Key error: node " << m_node << ":\n";
                ss << "does not have key `preset-name`";
                return std::unexpected(ss.str());
            }
            // field 'preset-name' was found. Tries to convert it to string
            ConfigNode presets_node = m_presets_node.value();
            utils::StrExpected<std::string> preset_name =
                node_preset_name.value().as<std::string>();
            if (!preset_name.has_value()) {
                // field 'preset-name' couldn't be converted to string. Returns
                // message about it
                std::stringstream ss;
                ss << "Error: field 'preset-name' in node " << m_node << '\n';
                ss << "couldn't be converted to string";
                return std::unexpected(ss.str());
            }
            // conversion is successful. tries to find preset in m_presets_node
            ConfigNodeExpected preset_node = presets_node[preset_name.value()];
            if (!preset_node.has_value()) {
                // m_presets_node hasn't preset with the specified name in
                // config. Retruns message about it
                std::stringstream ss;
                ss << "Error: preset named '" << preset_name.value()
                   << "' of node " << m_node << '\n';
                ss << "can not be found in presets node:" << presets_node
                   << ":\n";
                return std::unexpected(ss.str());
            }
            // preset was found successfull. put the found value in m_preset
            m_preset.emplace(preset_node.value());
        }
        // tries to find key in preset
        ConfigNodeExpected key_node = m_preset.value()[key];
        if (!key_node.has_value()) {
            // m_preset hasn't key. Returns message about it
            std::stringstream ss;
            ss << "Key error: preset '" << m_preset.value().get_name().value()
               << "' ";
            ss << "does not have key: " << key;
            return std::unexpected(ss.str());
        }
        // key was found in preset
        return ConfigNodeWithPreset(key_node.value(), m_presets_node, m_preset);
    }
    // key was found in m_node
    return ConfigNodeWithPreset(child_node.value(), m_presets_node, m_preset);
}

const std::string& ConfigNodeWithPreset::get_name_or_throw() const {
    return m_node.get_name_or_throw();
}

std::runtime_error ConfigNodeWithPreset::create_parsing_error(
    std::string_view error) const {
    return m_node.create_parsing_error(std::move(error));
}

const ConfigNode& ConfigNodeWithPreset::get_node() const noexcept {
    return m_node;
}

}  // namespace sim
