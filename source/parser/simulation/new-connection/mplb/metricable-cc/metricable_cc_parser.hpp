#pragma once
#include "connection/flow/tcp/metricable_cc.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {

MetricableCC parse_metricable_cc(const ConfigNodeWithPreset& node);

}
