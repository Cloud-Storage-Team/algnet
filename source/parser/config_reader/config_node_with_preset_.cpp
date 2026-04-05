#include "config_node_with_preset.hpp"

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
                ss << "does not have key '" << key << "';\n";
                ss << "Could not fild preset for it because `preset-name` does "
                      "not set";
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
                // config. Returns message about it
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
        // key was found in preset; use key_node as main node; DROP PRESET NODE
        return ConfigNodeWithPreset(key_node.value(), m_presets_node,
                                    std::nullopt);
    }
    // key was found in m_node: go to child value; DROP PRESET NODE
    return ConfigNodeWithPreset(child_node.value(), m_presets_node,
                                std::nullopt);
}

bool ConfigNodeWithPreset::IsMap() const noexcept { return m_node.IsMap(); }

std::ostream& operator<<(std::ostream& out, const ConfigNodeWithPreset& node) {
    return out << node.get_node();
}

const std::string& ConfigNodeWithPreset::get_name_or_throw() const {
    return m_node.get_name_or_throw();
}

std::runtime_error ConfigNodeWithPreset::create_parsing_error(
    std::string_view error) const {
    return m_node.create_parsing_error(error);
}

const ConfigNode& ConfigNodeWithPreset::get_node() const noexcept {
    return m_node;
}

[[nodiscard]] const std::optional<std::string>& ConfigNodeWithPreset::get_name()
    const {
    return m_node.get_name();
}

const std::optional<ConfigNode> ConfigNodeWithPreset::get_presets_node()
    const noexcept {
    return m_presets_node;
}

ConfigNodeWithPreset load_file_with_presets(std::filesystem::path path) {
    ConfigNode node = load_file(path);
    return ConfigNodeWithPreset(node, node["presets"].to_optional());
}

ConfigNodeWithPreset::Iterator::Iterator(YAML::const_iterator a_it) : m_iterator(a_it) {}

ConfigNodeWithPreset::Iterator& ConfigNodeWithPreset::Iterator::operator++() {
    ++m_iterator;
    return *this;
}

ConfigNodeWithPreset::Iterator ConfigNodeWithPreset::Iterator::operator++(int) {
    Iterator iterator_copy(*this);
    ++(*this);
    return iterator_copy;
}

bool ConfigNodeWithPreset::Iterator::operator==(const Iterator& rhs) const {
    return m_iterator == rhs.m_iterator;
}

bool ConfigNodeWithPreset::Iterator::operator!=(const Iterator& rhs) const {
    return m_iterator != rhs.m_iterator;
}

ConfigNodeWithPreset ConfigNodeWithPreset::Iterator::operator*() const {
    // Explanation: iterator_value under *m_iterator inherited from
    // YAML::Node and std::pair<YAML::Node, YAML::Node>, but only one value of
    // this pair is correct (please, tell them about std::variant...)
    if (m_iterator->IsDefined()) {
        // iterator goes over "named" nodes like
        // list:
        //  - value_0
        //  - value_1
        //  ...
        YAML::Node node = *m_iterator;
        return ConfigNodeWithPreset(ConfigNode(node, std::nullopt));
    } else {
        // iterator goes over "named" nodes like
        // list:
        //  node_1: ...
        //  node_2: ...
        //  ...
        YAML::Node key_node = m_iterator->first;
        YAML::Node value_node = m_iterator->second;
        if (!key_node || !value_node) {
            throw std::runtime_error(
                "Can not take value under config node iterator; all possible "
                "nodes are invalid");
        }
        std::string key = key_node.as<std::string>();
        return ConfigNodeWithPreset(ConfigNode(value_node, key));
    }
}

ConfigNodeWithPreset::Iterator ConfigNodeWithPreset::begin() const {
    return Iterator(m_node.get_node().begin());
}

ConfigNodeWithPreset::Iterator ConfigNodeWithPreset::end() const {
    return Iterator(m_node.get_node().end());
}


}  // namespace sim
