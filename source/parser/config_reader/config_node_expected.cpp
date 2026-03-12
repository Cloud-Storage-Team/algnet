#include "config_node.hpp"

namespace sim {

ConfigNodeExpected::ConfigNodeExpected(utils::StrExpected<ConfigNode> a_node)
    : utils::StrExpected<ConfigNode>(std::move(a_node)) {}

[[nodiscard]] utils::StrExpected<std::string> ConfigNodeExpected::get_name()
    const {
    if (!this->has_value()) {
        return std::unexpected(this->error());
    }
    const ConfigNode& node = this->value();
    const std::optional<std::string>& opt_name = node.get_name();
    if (opt_name) {
        return utils::StrExpected<std::string>(opt_name.value());
    } else {
        std::stringstream ss;
        ss << "Node " << node << " does not have name";
        return std::unexpected(ss.str());
    }
}

[[nodiscard]] ConfigNodeExpected ConfigNodeExpected::operator[](
    std::string_view key) const {
    if (!this->has_value()) {
        return *this;
    }
    return this->value()[key];
}

}  // namespace sim