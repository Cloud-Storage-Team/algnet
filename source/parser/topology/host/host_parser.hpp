#pragma once
#include <spdlog/fmt/fmt.h>

#include <memory>

#include "parser/config_reader/config_node_with_preset.hpp"
#include "topology/device/host.hpp"

namespace sim {

std::shared_ptr<IHost> parse_host(const ConfigNodeWithPreset& preset, const Id& id);   

class HostParser {
public:
    static std::shared_ptr<IHost> parse_i_host(
        const ConfigNodeWithPreset& host_node);
}; 

}  // namespace sim
