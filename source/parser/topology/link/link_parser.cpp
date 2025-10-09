#include "link_parser.hpp"

namespace sim {

std::shared_ptr<ILink> LinkParser::parse_i_link(const ConfigNode& link_node,
                                                const LinkPresets& presets) {
    return parse_default_link(link_node, presets);
}

void LinkParser::parse_to_args(const ConfigNode& node, LinkInitArgs& args) {
    node["from"].apply_if_present([&args](const ConfigNode& from) {
        args.from_id.emplace(from.as_or_throw<Id>());
    });

    node["to"].apply_if_present([&args](const ConfigNode& to) {
        args.to_id.emplace(to.as_or_throw<Id>());
    });

    node["latency"].apply_if_present([&args](const ConfigNode& latency) {
        args.delay.emplace(parse_time(latency));
    });

    node["throughput"].apply_if_present([&args](const ConfigNode& speed) {
        args.speed.emplace(parse_speed(speed));
    });

    node["egress_buffer_size"].apply_if_present(
        [&args](const ConfigNode& max_from_egress_buffer_size) {
            args.max_from_egress_buffer_size.emplace(
                parse_size(max_from_egress_buffer_size));
        });

    node["ingress_buffer_size"].apply_if_present(
        [&args](const ConfigNode& max_to_ingress_buffer_size) {
            args.max_to_ingress_buffer_size.emplace(
                parse_size(max_to_ingress_buffer_size));
        });
}

std::shared_ptr<Link> LinkParser::parse_default_link(
    const ConfigNode& link_node, const LinkPresets& presets) {
    Id link_id = link_node.get_name_or_throw();
    LinkInitArgs link_args = presets.get_preset(link_node);
    parse_to_args(link_node, link_args);
    link_args.id = link_id;
    try {
        return std::make_shared<Link>(std::move(link_args));
    } catch (const std::exception& e) {
        throw link_node.create_parsing_error(e.what());
    }
}

}  // namespace sim
