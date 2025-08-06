#include "link_parser.hpp"

#include "parser/parse_utils.hpp"

namespace sim {

std::shared_ptr<ILink> LinkParser::parse_i_link(const YAML::Node& key_node,
                                                const YAML::Node& value_node,
                                                LinkInitArgs preset_args) {
    return parse_default_link(key_node, value_node, std::move(preset_args));
}

void LinkParser::parse_to_args(const YAML::Node& node, LinkInitArgs& args) {
    auto from = node["from"];
    if (from) {
        args.from_id.emplace(from.as<Id>());
    }

    auto to = node["to"];
    if (to) {
        args.to_id.emplace(to.as<Id>());
    }

    auto latency = node["latency"];
    if (latency) {
        args.delay.emplace(parse_time(latency.as<std::string>()));
    }

    auto speed = node["throughput"];
    if (speed) {
        args.speed.emplace(parse_speed(speed.as<std::string>()));
    }

    auto max_from_egress_buffer_size = node["egress_buffer_size"];
    if (max_from_egress_buffer_size) {
        args.max_from_egress_buffer_size.emplace(
            parse_buffer_size(max_from_egress_buffer_size.as<std::string>()));
    }

    auto max_to_ingress_buffer_size = node["ingress_buffer_size"];
    if (max_to_ingress_buffer_size) {
        args.max_to_ingress_buffer_size.emplace(
            parse_buffer_size(max_to_ingress_buffer_size.as<std::string>()));
    }
}

std::shared_ptr<Link> LinkParser::parse_default_link(
    const YAML::Node& key_node, const YAML::Node& value_node,
    LinkInitArgs args) {
    parse_to_args(value_node, args);
    args.id.emplace(key_node.as<Id>());
    return std::make_shared<Link>(std::move(args));
}

}  // namespace sim
