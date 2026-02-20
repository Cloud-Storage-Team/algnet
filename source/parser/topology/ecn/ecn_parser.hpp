#pragma once
#pragma once
#include <spdlog/fmt/fmt.h>

#include <memory>

#include "device/ecn.hpp"
#include "parser/config_reader/common_config_node_with_preset.hpp"

namespace sim {

class EcnParser {
public:
    static ECN parse_ecn(const ConfigNodeWithPreset& node);
};

}  // namespace sim
