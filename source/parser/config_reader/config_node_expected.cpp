#include "common_config_node.hpp"

namespace sim{

ConfigNodeExpected::ConfigNodeExpected(utils::StrExpected<ConfigNode> a_node): utils::StrExpected<ConfigNode>(std::move(a_node)){}

[[nodiscard]] utils::StrExpected<std::string> ConfigNodeExpected::get_name() const{
    if (!this->has_value()){
        return std::unexpected(this->error());
    }
    try{
        return utils::StrExpected<std::string>(this->value().template get_name_or_throw());
    } catch (const std::exception& e){
        return std::unexpected(std::string(e.what()));
    }
}

ConfigNodeExpected ConfigNodeExpected::operator[] (std::string_view key) const{
    if (!this->has_value()){
        return std::unexpected("The operator [] was called on an empty object.");
    }
    return this->value()[key];
}


} // namespace sim