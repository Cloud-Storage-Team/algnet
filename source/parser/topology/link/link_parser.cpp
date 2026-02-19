#include "./link_parser.hpp"

#include "parser/parse_utils.hpp"

namespace sim {

std::shared_ptr<ILink> LinkParser::parse_i_link(
    const ConfigNodeWithPreset& link_node) {
    return parse_link(link_node);
}

std::shared_ptr<Link> LinkParser::parse_link(
    const ConfigNodeWithPreset& link_node) {
    Id id = link_node.get_name_or_throw();

    auto& idf = IdentifierFactory::get_instance();

    Id from_id = link_node["from"].value_or_throw().as_or_throw<Id>();
    std::weak_ptr<IDevice> from_ptr = idf.get_object<IDevice>(from_id);

    Id to_id = link_node["to"].value_or_throw().as_or_throw<Id>();
    std::weak_ptr<IDevice> to_ptr = idf.get_object<IDevice>(to_id);

    SpeedGbps speed =
        parse_speed(link_node["throughput"].value_or_throw().get_node());
    TimeNs delay = parse_time(link_node["latency"].value_or_throw().get_node());
    SizeByte egress_buffer_size =
        parse_size(link_node["egress_buffer_size"].value_or_throw().get_node());
    SizeByte ingress_buffer_size = parse_size(
        link_node["ingress_buffer_size"].value_or_throw().get_node());
    return std::make_shared<Link>(id, from_ptr, to_ptr, speed, delay,
                                  egress_buffer_size, ingress_buffer_size);
}

}  // namespace sim
