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

ConfigNodeExpected safe_load_file(std::filesystem::path path) noexcept {
    try {
        return ConfigNode(YAML::LoadFile(path.string()), std::nullopt, path);
    } catch (const std::exception& ex) {
        return std::unexpected(
            fmt::format("Failed to parse file at path: {}, due to error: {}",
                        path.string(), ex.what()));
    }
}

}  // namespace sim