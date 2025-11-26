#include <ranges>
#include <regex>

#include "action_parser.hpp"
#include "scenario/action/send_data_action.hpp"

namespace sim {

// Get target connections from the YAML node
static std::vector<std::weak_ptr<IConnection>> get_target_connections(
    std::regex re) {
    std::vector<std::weak_ptr<IConnection>> conns;

    auto& factory = IdentifierFactory::get_instance();
    for (const auto& conn : factory.get_objects<IConnection>()) {
        if (std::regex_match(conn->get_id(), re)) {
            conns.push_back(conn);
        }
    }
    return conns;
}

std::unique_ptr<IAction> ActionParser::parse_send_data(const ConfigNode& node) {
    const TimeNs when = parse_time(node["when"].value_or_throw());
    const SizeByte size = parse_size(node["size"].value_or_throw());
    const std::regex connections =
        parse_regex(node["connections"].value_or_throw());

    const std::uint32_t repeat_count =
        simple_parse_with_default(node, "repeat_count", 1u);

    auto repeat_interval_node = node["repeat_interval"];
    const TimeNs repeat_interval =
        (repeat_interval_node ? parse_time(repeat_interval_node.value())
                              : TimeNs(0));
    auto jitter_node = node["jitter"];
    const TimeNs jitter =
        (jitter_node ? parse_time(jitter_node.value()) : TimeNs(0));
    auto conns = get_target_connections(connections);

    if (conns.empty()) {
        throw node.create_parsing_error(
            "No connections specified for send data action");
    }

    std::shared_ptr<SendDataActionsSummary> summary = m_summary;
    SizeByte total_size = size * repeat_count * conns.size();
    TimeNs start_time = when;
    // TODO: fill id correcrly
    DataId data_id = "dataid_changeme";

    OnDeliveryCallback callback = [data_id, total_size, start_time, summary] {
        TimeNs finish_time = Scheduler::get_instance().get_current_time();
        summary->emplace_back(SendDataActionsSummaryRow{
            std::move(data_id), total_size, start_time, finish_time});
    };

    return std::make_unique<SendDataAction>(when, size, conns, repeat_count,
                                            repeat_interval, jitter, callback);
}

}  // namespace sim
