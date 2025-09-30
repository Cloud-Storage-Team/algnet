#include <ranges>
#include <regex>

#include "action_parser.hpp"
#include "scenario/action/send_data_action.hpp"

namespace sim {

// Get target connections from the YAML node
static std::vector<std::weak_ptr<IConnection>> get_target_connections(
    const YAML::Node& node) {
    std::vector<std::weak_ptr<IConnection>> conns;
    auto& factory = IdentifierFactory::get_instance();

    const auto cs = node["connections"];
    if (!cs || !cs.IsScalar()) {
        throw std::runtime_error(
            "send_data requires `connections` as a scalar string");
    }
    const std::string pattern = cs.as<std::string>();
    const std::regex re(pattern);

    for (const auto& conn : factory.get_objects<IConnection>()) {
        if (std::regex_match(conn->get_id(), re)) {
            conns.push_back(conn);
        }
    }
    if (conns.empty()) {
        throw std::runtime_error("No connections matched regex: " + pattern);
    }
    return conns;
}

std::unique_ptr<IAction> ActionParser::parse_send_data(const YAML::Node& node) {
    auto require_field = [&node](const std::string& key) -> YAML::Node {
        if (!node[key])
            throw std::runtime_error("send_data requires `" + key + "`");
        return node[key];
    };
    const TimeNs when = parse_time(require_field("when").as<std::string>());
    const SizeByte size = parse_size(require_field("size").as<std::string>());
    const YAML::Node cs = require_field("connections");

    const int repeat_count =
        node["repeat_count"] ? node["repeat_count"].as<int>() : 1;
    if (repeat_count <= 0)
        throw std::runtime_error("`repeat_count` must be > 0");

    const TimeNs repeat_interval =
        node["repeat_interval"]
            ? parse_time(node["repeat_interval"].as<std::string>())
            : TimeNs(0);
    auto conns = get_target_connections(node);

    return std::make_unique<SendDataAction>(when, size, conns, repeat_count,
                                            repeat_interval);
}

}  // namespace sim
