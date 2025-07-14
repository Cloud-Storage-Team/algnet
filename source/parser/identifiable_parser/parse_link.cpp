#include <functional>

#include "identifiable_parser.hpp"
#include "link/link.hpp"
#include "parser/parse_primitives.hpp"

namespace sim {

static uint32_t parse_with_default(
    const YAML::Node& node, std::string_view field_name,
    std::function<uint32_t(const std::string&)> value_parser,
    uint32_t default_value) {
    if (!node[field_name]) {
        LOG_WARN(fmt::format("{} does not set ; use default value {}",
                             field_name, default_value));
        return default_value;
    }
    return value_parser(node[field_name].as<std::string>());
}

template <>
std::shared_ptr<Link> IdentifieableParser::parse_object<Link>(
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