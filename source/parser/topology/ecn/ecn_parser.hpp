#pragma once
#pragma once
#include <spdlog/fmt/fmt.h>

#include <memory>

#include "parser/config_reader/config_node_with_preset.hpp"
#include "topology/device/ecn.hpp"

namespace sim {

class EcnParser {
public:
    static ECN parse_ecn(const ConfigNodeWithPreset& node);
};

}  // namespace sim
