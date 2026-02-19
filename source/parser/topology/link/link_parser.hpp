#pragma once
#include "link/link.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

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
