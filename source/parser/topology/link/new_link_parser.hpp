#pragma once

#pragma once
#include "link/link.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"
#include "utils/id_table.hpp"

namespace sim {

std::shared_ptr<ILink> new_parse_i_link(
    const ConfigNodeWithPreset& link_node,
    const utils::IdTable<IDevice>& device_table);

}  // namespace sim
