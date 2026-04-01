#include "link_parser.hpp"

#include "parser/parse_utils.hpp"

namespace sim {

static LinkMetricsFilters parse_metrics_filters(
    const ConfigNodeWithPresetExpected& node) {
    LinkMetricsFilters filters = Link::DEFAULT_METRICS_FILTERS;
    if (const auto& queue_sizes_node = node["queue_sizes"]; queue_sizes_node) {
        filters.queues_size = queue_sizes_node.value().as_or_throw<bool>();
    }
    return filters;
}

std::shared_ptr<ILink> parse_i_link(std::shared_ptr<IDevice> from,
                                    std::shared_ptr<IDevice> to, const Id& id,
                                    const ConfigNodeWithPreset& link_preset) {
    const SpeedGbps speed =
        parse_speed(link_preset["throughput"].value_or_throw().get_node());
    const TimeNs delay =
        parse_time(link_preset["latency"].value_or_throw().get_node());
    SizeByte egress_buffer_size = parse_size(
        link_preset["egress_buffer_size"].value_or_throw().get_node());
    SizeByte ingress_buffer_size = parse_size(
        link_preset["ingress_buffer_size"].value_or_throw().get_node());

    LinkMetricsFilters metrics_filters =
        parse_metrics_filters(link_preset["metrics_filters"]);

    return Link::create_shared(id, from, to, speed, delay, egress_buffer_size,
                               ingress_buffer_size, metrics_filters);
}

std::shared_ptr<ILink> parse_i_link(std::shared_ptr<IDevice> from,
                                    std::shared_ptr<IDevice> to,
                                    const ConfigNodeWithPreset& link_preset) {
    Id id = fmt::format("{}--{}", from->get_id(), to->get_id());

    return parse_i_link(from, to, id, link_preset);
}

std::shared_ptr<ILink> parse_i_link(
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

    return parse_i_link(from_ptr, to_ptr, id, link_node);
}
}  // namespace sim
