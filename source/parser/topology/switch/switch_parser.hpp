#pragma once
#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

#include <memory>

#include "device/switch.hpp"

namespace sim {

using SwitchPresets = PresetStorage<SwitchInitArgs>;

class SwitchParser {
public:
    static std::shared_ptr<ISwitch> parse_i_switch(
        const YAML::Node& key_node, const YAML::Node& value_node,
        const YAML::Node& packet_spraying_node, const SwitchPresets& presets);

    static void parse_to_args(const YAML::Node& node, SwitchInitArgs& args);

private:
    static std::shared_ptr<Switch> parse_default_switch(
        const YAML::Node& key_node, const YAML::Node& value_node,
        const YAML::Node& packet_spraying_node, const SwitchPresets& presets);

    static ECN parse_ecn(const YAML::Node& node);

    static std::unique_ptr<IPacketHasher> parse_hasher(
        const YAML::Node& packet_spraying_node, Id switch_id);
};

}  // namespace sim
