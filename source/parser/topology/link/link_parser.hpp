#pragma once
#include "link/link.hpp"
#include "parser/preset_storage.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {

using LinkPresets = PresetStorage<LinkInitArgs>;

class LinkParser {
public:
    static std::shared_ptr<ILink> parse_i_link(const ConfigNodeWithPreset& link_node);

    static std::shared_ptr<Link> parse_to_args(const ConfigNodeWithPreset& node);

private:
    static std::shared_ptr<Link> parse_link(const ConfigNodeWithPreset& link_node);
};

}  // namespace sim
