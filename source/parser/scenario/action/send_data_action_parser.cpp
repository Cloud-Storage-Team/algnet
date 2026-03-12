#include "send_data_action_parser.hpp"

#include <ranges>
#include <regex>

#include "network/connection/i_connection.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

// Get target connections from the YAML node
static std::vector<std::shared_ptr<IConnection>> get_target_connections(
    std::regex re, const utils::IdTable<IConnection> connections_table) {
    std::vector<std::shared_ptr<IConnection>> result;
    for (const auto& [id, connection] : connections_table) {
        if (std::regex_match(connection->get_id(), re)) {
            result.emplace_back(connection);
        }
    }
    return result;
}

std::shared_ptr<SendDataAction> parse_send_data_action(
    const ConfigNode& node,
    const utils::IdTable<IConnection> connections_table) {
    const TimeNs when = parse_time(node["when"].value_or_throw());
    const SizeByte size = parse_size(node["size"].value_or_throw());
    const std::regex connections_regex =
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
    auto conns = get_target_connections(connections_regex, connections_table);

    if (conns.empty()) {
        throw node.create_parsing_error(
            "No connections specified for send data action");
    }

    RawDataId data_id = node["id"].value_or_throw().as_or_throw<RawDataId>();

    return std::make_shared<SendDataAction>(
        when, size, data_id, conns, repeat_count, repeat_interval, jitter);
}

}  // namespace sim
