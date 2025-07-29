#include "identifiable_parser.hpp"
#include "link/link.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

template <>
std::shared_ptr<Link> Parser<Link>::parse_object(const YAML::Node& key_node,
                                                 const YAML::Node& value_node) {
    Id id = key_node.as<Id>();
    Id from_id = value_node["from"].as<Id>();
    Id to_id = value_node["to"].as<Id>();
    auto from_ptr =
        IdentifierFactory::get_instance().get_object<IDevice>(from_id);
    auto to_ptr = IdentifierFactory::get_instance().get_object<IDevice>(to_id);

    if (from_ptr == nullptr) {
        LOG_ERROR("Failed to find link's source");
        return nullptr;
    }

    if (to_ptr == nullptr) {
        LOG_ERROR("Failed to find link's destination");
        return nullptr;
    }

    TimeNs latency = parse_with_default<TimeNs>(value_node, "latency",
                                                parse_time, TimeNs(0));

    SpeedGbps speed = parse_with_default<SpeedGbps>(value_node, "throughput",
                                                    parse_speed, SpeedGbps(1));

    SizeByte ingress_buffer_size = parse_with_default<SizeByte>(
        value_node, "ingress_buffer_size", parse_buffer_size, SizeByte(4096u));

    SizeByte egress_buffer_size = parse_with_default<SizeByte>(
        value_node, "egress_buffer_size", parse_buffer_size, SizeByte(4096u));

    return std::make_shared<Link>(id, from_ptr, to_ptr, speed, latency,
                                  egress_buffer_size, ingress_buffer_size);
}
}  // namespace sim
