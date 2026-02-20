#include "config_node_with_preset_expected.hpp"

namespace sim{

ConfigNodeWithPresetExpected::ConfigNodeWithPresetExpected(ConfigNodeWithPreset a_node): utils::StrExpected<ConfigNode>(std::move(a_node)){}

template<typename T>
[[nodiscard]] utils::StrExpected<T> ConfigNodeWithPresetExpected::as<T>() const noexcept{
    if (!this->has_value()){
        return std::unexpected(this->error());
    }
    try{
        return utils:StrExpected<T>(this->value().template as<T>());
    } catch (const std::exception& e){
        return std::unexpected(std::string(e.what()));
    }
}

[[nodiscard]] utils::StrExpected<std::string> ConfigNodeWithPresetExpected::get_name() const{
    if (!this->has_value()){
        return std::unexpected(this->error());
    }
    try{
        return utils::StrExpected<std::string>(this->value().template get_name_or_throw());
    } catch (const std::exception& e){
        return std::unexpected(std::string(e.what()));
    }
}

ConfigNodeWithPresetExpected ConfigNodeWithPresetExpected::operator[] (std::string_view key) const{
    if (!this->has_value()){
        return std::unexpected("The operator [] was called on an empty object.");
    }
    return this->value()[key];
}

} // namespace sim