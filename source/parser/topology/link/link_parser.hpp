#pragma once

#pragma once
#include "parser/config_reader/config_node_with_preset.hpp"
#include "topology/link/link.hpp"
#include "utils/id_table.hpp"

namespace sim {

std::shared_ptr<ILink> parse_i_link(
    const ConfigNodeWithPreset& link_node,
    const utils::IdTable<IDevice>& device_table);

}  // namespace sim
