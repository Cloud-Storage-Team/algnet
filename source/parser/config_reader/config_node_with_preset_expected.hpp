#pragma once

#include <optional>

#include "utils/str_expected.hpp"

namespace sim {

class ConfigNodeWithPreset;

class ConfigNodeWithPresetExpected
    : public utils::StrExpected<ConfigNodeWithPreset> {
public:
    using utils::StrExpected<ConfigNodeWithPreset>::StrExpected;

    ConfigNodeWithPresetExpected(ConfigNodeWithPreset a_node);

    template <typename T>
    [[nodiscard]] utils::StrExpected<T> as() const noexcept {
        if (!this->has_value()) {
            return std::unexpected(this->error());
        }
        return utils::StrExpected<T>(this->value().template as<T>());
    }

    [[nodiscard]] utils::StrExpected<std::string> get_name() const;

    [[nodiscard]] ConfigNodeWithPresetExpected operator[](
        std::string_view key) const;
};

}  // namespace sim
