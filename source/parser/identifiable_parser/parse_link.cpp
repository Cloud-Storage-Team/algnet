#include "identifiable_parser.hpp"
#include "link/link.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

template <>
std::shared_ptr<Link> Parser<Link>::parse_object(
    const YAML::Node& key_node, const YAML::Node& value_node) {
    Id id = key_node.as<Id>();
    Id from_id = value_node["from"].as<Id>();
    Id to_id = value_node["to"].as<Id>();
    auto from_ptr =
        IdentifierFactory::get_instance().get_object<IRoutingDevice>(from_id);
    auto to_ptr =
        IdentifierFactory::get_instance().get_object<IRoutingDevice>(to_id);

    if (from_ptr == nullptr) {
        LOG_ERROR("Failed to find link's source");
        return nullptr;
    }

    if (to_ptr == nullptr) {
        LOG_ERROR("Failed to find link's destination");
        return nullptr;
    }

    uint32_t latency =
        parse_with_default(value_node, "latency", parse_latency, 0u);

    uint32_t speed =
        parse_with_default(value_node, "throughput", parse_throughput, 1u);

    uint32_t ingress_buffer_size = parse_with_default(
        value_node, "ingress_buffer_size", parse_buffer_size, 4096u);

    uint32_t egress_buffer_size = parse_with_default(
        value_node, "egress_buffer_size", parse_buffer_size, 4096u);

    return std::make_shared<Link>(id, from_ptr, to_ptr, speed, latency,
                                  egress_buffer_size, ingress_buffer_size);
}
}  // namespace sim