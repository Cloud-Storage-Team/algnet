#pragma once
#include "network/connection/rdma/dcqcn/dcqcn.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {
DCQCN parse_dcqcn(const ConfigNodeWithPreset& node);
}