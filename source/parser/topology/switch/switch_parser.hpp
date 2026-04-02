#pragma once
#include <spdlog/fmt/fmt.h>

#include <memory>

#include "parser/config_reader/config_node_with_preset.hpp"
#include "topology/device/switch.hpp"

namespace sim {

std::shared_ptr<ISwitch> parse_switch(const ConfigNodeWithPreset& preset,
                                      const ConfigNodeWithPreset& packet_spraying_node,
                                      const Id& id);

class SwitchParser {
public:
    static std::shared_ptr<ISwitch> parse_i_switch(
        const ConfigNodeWithPreset& switch_node,
        const ConfigNodeWithPreset& packet_spraying_node);
};

}  // namespace sim
