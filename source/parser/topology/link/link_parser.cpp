#include "link_parser.hpp"

#include "parser/parse_utils.hpp"

namespace sim {

static std::shared_ptr<Link> parse_link(
    const ConfigNodeWithPreset& link_node,
    const utils::IdTable<IDevice>& device_table);

std::shared_ptr<ILink> parse_i_link(
    const ConfigNodeWithPreset& link_node,
    const utils::IdTable<IDevice>& device_table) {
    return parse_link(link_node, device_table);
}

static LinkMetricsFilters parse_metrics_filters(
    const ConfigNodeWithPresetExpected& node) {
    LinkMetricsFilters filters = Link::DEFAULT_METRICS_FILTERS;
    if (const ConfigNodeWithPresetExpected& queue_sizes_node =
            node["queue_sizes"];
        queue_sizes_node) {
        filters.queues_size = queue_sizes_node.value().as_or_throw<bool>();
    }
    return filters;
}

static std::shared_ptr<Link> parse_link(
    const ConfigNodeWithPreset& link_node,
    const utils::IdTable<IDevice>& device_table) {
    const Id id = link_node.get_name_or_throw();

    const Id from_id = link_node["from"].value_or_throw().as_or_throw<Id>();
    std::shared_ptr<IDevice> from_ptr = device_table.get_or_throw(
        from_id,
        link_node.create_parsing_error(fmt::format(
            "Could not find link's source device with id '{}'", from_id)));

    const Id to_id = link_node["to"].value_or_throw().as_or_throw<Id>();
    std::shared_ptr<IDevice> to_ptr = device_table.get_or_throw(
        to_id,
        link_node.create_parsing_error(fmt::format(
            "Could not find link's destination device with id '{}'", to_id)));

    const SpeedGbps speed =
        parse_speed(link_node["throughput"].value_or_throw().get_node());
    const TimeNs delay =
        parse_time(link_node["latency"].value_or_throw().get_node());
    SizeByte egress_buffer_size =
        parse_size(link_node["egress_buffer_size"].value_or_throw().get_node());
    SizeByte ingress_buffer_size = parse_size(
        link_node["ingress_buffer_size"].value_or_throw().get_node());

    LinkMetricsFilters metrics_filters =
        parse_metrics_filters(link_node["metrics_filters"]);

    return std::make_shared<Link>(id, from_ptr, to_ptr, speed, delay,
                                  egress_buffer_size, ingress_buffer_size,
                                  metrics_filters);
}

}  // namespace sim
