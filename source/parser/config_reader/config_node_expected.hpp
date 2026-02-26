#pragma once

#include "utils/str_expected.hpp"

namespace sim {

class ConfigNode;

class ConfigNodeExpected : public utils::StrExpected<ConfigNode> {
public:
    using utils::StrExpected<ConfigNode>::StrExpected;

    ConfigNodeExpected(utils::StrExpected<ConfigNode> a_node);

    template <typename T>
    [[nodiscard]] utils::StrExpected<T> as() const noexcept {
        if (!this->has_value()) {
            return std::unexpected(this->error());
        }
        return utils::StrExpected<T>(this->value().template as<T>());
    }

    [[nodiscard]] utils::StrExpected<std::string> get_name() const;

    [[nodiscard]] ConfigNodeExpected operator[](std::string_view key) const;
};

}  // namespace sim