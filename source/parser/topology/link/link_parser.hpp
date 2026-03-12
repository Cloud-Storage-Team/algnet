#pragma once
#include "parser/config_reader/config_node_with_preset.hpp"
#include "topology/link/link.hpp"

namespace sim {

class LinkParser {
public:
    static std::shared_ptr<ILink> parse_i_link(
        const ConfigNodeWithPreset& link_node);

private:
    static std::shared_ptr<Link> parse_link(
        const ConfigNodeWithPreset& link_node);
};

}  // namespace sim
