#pragma once

#include "utils/str_expected.hpp"
#include "./config_node_with_preset.hpp"

namespace sim{

class ConfigNodeWithPreset;

struct ConfigNodeWithPresetExpected: utils::StrExpected<ConfigNodeWithPreset>{
    
    CondigNodeWithPresetExpected(ConfigNodeWithPreset a_node);

    template<typename T>
    [[nodiscard]] utils::StrExpected<T> as() const noexcept;

    [[nodiscard]] utils::StrExpected<std::string> get_name() const;

    ConfigNodeWithPresetExpected operator[](std::string_view key) const;
};

} // namespace sim