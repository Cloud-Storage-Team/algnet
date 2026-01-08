#include "./config_node.hpp"

namespace sim{

class ConfigNodeWithPreset{
public:
    ConfigNodeWithPreset(ConfigNode a_node, ConfigNode a_presets_node);

    ConfigNodeWithPreset(ConfigNode a_node, ConfigNode a_preset, ConfigNode a_presets_node);

    void set_preset(const ConfigNode &a_preset);

    ConfigNodeWithPreset operator[](std::string_view key) const;

private:
    const ConfigNode m_node;
    std::optional<ConfigNode> m_preset;
    const ConfigNode m_presets_node;
};

} // namespace sim 