#include "./config_node_with_preset.hpp"

namespace sim{

ConfigNodeWithPreset::ConfigNodeWithPreset(ConfigNode a_node, ConfigNode a_preset_node)
    : m_node(std::move(a_node)), m_presets_node(std::move(a_preset_node)){}

ConfigNodeWithPreset::ConfigNodeWithPreset(ConfigNode a_node, ConfigNode a_preset, ConfigNode a_presets_node)
    : m_node(std::move(a_node)), m_preset(std::move(a_preset)), m_presets_node(std::move(a_presets_node)){}

void ConfigNodeWithPreset::set_preset(const ConfigNode &a_preset){
    m_preset = std::make_optional(a_preset);
}

ConfigNodeWithPreset ConfigNodeWithPreset::operator[](std::string_view key) const{

}

} // namespace sim