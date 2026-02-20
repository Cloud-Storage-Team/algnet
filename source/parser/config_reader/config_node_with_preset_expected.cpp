#include "common_config_node_with_preset.hpp"

namespace sim{

ConfigNodeWithPresetExpected::ConfigNodeWithPresetExpected(ConfigNodeWithPreset a_node): utils::StrExpected<ConfigNodeWithPreset>(std::move(a_node)){}

[[nodiscard]] utils::StrExpected<std::string> ConfigNodeWithPresetExpected::get_name() const{
    if (!this->has_value()){
        return std::unexpected(this->error());
    }
    try{
        return utils::StrExpected<std::string>(this->value().get_name_or_throw());
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