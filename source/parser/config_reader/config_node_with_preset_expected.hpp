#pragma once

#include "utils/str_expected.hpp"

namespace sim{

class ConfigNodeWithPreset;

struct ConfigNodeWithPresetExpected: utils::StrExpected<ConfigNodeWithPreset>{

    using utils::StrExpected<ConfigNodeWithPreset>::StrExpected;
    
    ConfigNodeWithPresetExpected(ConfigNodeWithPreset a_node);

    template<typename T>
    [[nodiscard]] utils::StrExpected<T> as() const noexcept{
        if (!this->has_value()){
            return std::unexpected(this->error());
        }
        try{
            return utils::StrExpected<T>(this->value().template as<T>());
        } catch (const std::exception& e){
            return std::unexpected(std::string(e.what()));
        }
    }

    [[nodiscard]] utils::StrExpected<std::string> get_name() const;

    ConfigNodeWithPresetExpected operator[](std::string_view key) const;


};

} // namespace sim