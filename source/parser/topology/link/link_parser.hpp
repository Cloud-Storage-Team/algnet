#pragma once
#include "link/link.hpp"
#include "parser/preset_storage.hpp"

namespace sim {

using LinkPresets = PresetStorage<LinkInitArgs>;

class LinkParser {
public:
    static std::shared_ptr<ILink> parse_i_link(const YAML::Node& key_node,
                                               const YAML::Node& value_node,
                                               const LinkPresets& presets);
    static void parse_to_args(const YAML::Node& node, LinkInitArgs& args);

private:
    static std::shared_ptr<Link> parse_default_link(
        const YAML::Node& key_node, const YAML::Node& value_node,
        const LinkPresets& presets);
};

}  // namespace sim
