#include "./link_parser.hpp"

namespace sim {

std::shared_ptr<ILink> LinkParser::parse_i_link(const ConfigNodeWithPreset& link_node) {
    return parse_link(link_node);
}

std::shared_ptr<Link> LinkParser::parse_to_args(const ConfigNodeWithPreset& node) {
    Id m_id = node.get_node().get_name_or_throw();
    std::weak_ptr<IDevice> m_from = IdentifierFactory::get_instance().get_object<IDevice>(node["from"].value_or_throw().get_node().as_or_throw<Id>());
    std::weak_ptr<IDevice> m_to = IdentifierFactory::get_instance().get_object<IDevice>(node["to"].value_or_throw().get_node().as_or_throw<Id>());
    SpeedGbps m_speed = parse_speed(node["throughput"].value_or_throw().get_node());
    TimeNs m_delay = parse_time(node["latency"].value_or_throw().get_node());
    SizeByte m_egress_buffer_size = parse_size(node["egress_buffer_size"].value_or_throw().get_node());
    SizeByte m_ingress_buffer_size = parse_size(node["ingress_buffer_size"].value_or_throw().get_node());
    return std::make_shared<Link>(std::move(Link(m_id, m_from, m_to, m_speed, m_delay, m_egress_buffer_size, m_ingress_buffer_size)));
}

std::shared_ptr<Link> LinkParser::parse_link(
    const ConfigNodeWithPreset& link_node) {
    try {
        return parse_to_args(link_node);
    } catch (const std::exception& e) {
        throw link_node.get_node().create_parsing_error(e.what());
    }
}

}  // namespace sim
