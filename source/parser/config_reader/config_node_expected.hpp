#pragma once

#include "utils/str_expected.hpp"
#include "./config_node.hpp"

namespace sim{

struct ConfigNodeExpected: utils::StrExpected<ConfigNode>{
    
    ConfigNodeExpected(ConfigNode a_node);

    template<typename T>
    [[nodiscard]] utils::StrExpected<T> as() const noexcept;

    [[nodiscard]] utils::StrExpected<std::string> get_name() const;

    ConfigNodeExpected operator[](std::string_view key) const;
};

} // namespace sim