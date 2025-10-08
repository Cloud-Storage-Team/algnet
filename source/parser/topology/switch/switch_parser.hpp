#pragma once
#include <spdlog/fmt/fmt.h>

#include <memory>

#include "device/switch.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

class SwitchParser {
public:
    static std::shared_ptr<ISwitch> parse_i_switch(
        const ConfigNode& switch_node, const ConfigNode& packet_spraying_node);

private:
    static std::shared_ptr<Switch> parse_default_switch(
        const ConfigNode& switch_node, const ConfigNode& packet_spraying_node);

    static ECN parse_ecn(const YAML::Node& node);

    static std::unique_ptr<IPacketHasher> parse_hasher(
        const YAML::Node& packet_spraying_node, Id switch_id);
};

}  // namespace sim
