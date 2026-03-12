#pragma once
#include "network/connection/mplb/path_chooser/i_path_chooser.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {

std::unique_ptr<IPathChooser> parse_i_path_chooser(
    const ConfigNodeWithPreset& node, Endpoints endpoints);

}
