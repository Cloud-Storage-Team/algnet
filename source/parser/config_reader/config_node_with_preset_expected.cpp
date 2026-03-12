#include "config_node_with_preset.hpp"

namespace sim {

ConfigNodeWithPresetExpected::ConfigNodeWithPresetExpected(
    ConfigNodeWithPreset a_node)
    : utils::StrExpected<ConfigNodeWithPreset>(std::move(a_node)) {}

[[nodiscard]] utils::StrExpected<std::string>
ConfigNodeWithPresetExpected::get_name() const {
    if (!this->has_value()) {
        return std::unexpected(this->error());
    }
    const ConfigNodeWithPreset& node = this->value();
    const std::optional<std::string>& opt_name = node.get_name();
    if (opt_name) {
        return utils::StrExpected<std::string>(opt_name.value());
    } else {
        std::stringstream ss;
        ss << "Node " << node << " doesn't have name";
        return std::unexpected(ss.str());
    }
}

[[nodiscard]] ConfigNodeWithPresetExpected
ConfigNodeWithPresetExpected::operator[](std::string_view key) const {
    if (!this->has_value()) {
        return *this;
    }
    return this->value()[key];
}

}  // namespace sim
