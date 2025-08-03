#pragma once
#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

#include <memory>

#include "device/switch.hpp"

namespace sim {

class ParseSwitch {
public:
    static std::shared_ptr<ISwitch> parse_i_switch(const YAML::Node& key_node,
                                                 const YAML::Node& value_node);
private:
    static std::shared_ptr<Switch> parse_default_switch(const YAML::Node& key_node,
                                                 const YAML::Node& value_node);

    static ECN parse_ecn(const YAML::Node& node);
};

}  // namespace sim
