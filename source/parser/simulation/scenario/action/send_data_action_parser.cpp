#include "send_data_action_parser.hpp"

#include <ranges>
#include <regex>

#include "parser/parse_utils.hpp"

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

SendDataActionParser::SendDataActionParser(
    std::shared_ptr<SendDataActionsSummary> a_summary)
    : m_summary(a_summary) {}

std::unique_ptr<IAction> SendDataActionParser::parse(const ConfigNode& node) {
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

    RawDataId data_id = node["id"].value_or_throw().as_or_throw<RawDataId>();
    if (m_data_ids.contains(data_id)) {
        throw node.create_parsing_error(
            fmt::format("Action with id '{}' already exists", data_id));
    }
    m_data_ids.insert(data_id);

    std::shared_ptr<SendDataActionsSummary> summary = m_summary;
    std::vector<Id> connection_ids(conns.size());
    for (size_t i = 0; i < connection_ids.size(); i++) {
        connection_ids[i] = conns[i].lock()->get_id();
    }

    return std::make_unique<SendDataAction>(when, size, data_id, conns,
                                            repeat_count, repeat_interval,
                                            jitter, summary);
}

}  // namespace sim