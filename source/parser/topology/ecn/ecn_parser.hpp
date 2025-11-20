#pragma once
#pragma once
#include <spdlog/fmt/fmt.h>

#include <memory>

#include "device/ecn.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

class EcnParser {
public:
    static ECN parse_ecn(const ConfigNode& node);
};

}  // namespace sim
