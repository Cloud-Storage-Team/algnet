#include "send_data_conn_handler.hpp"

#include <regex>
#include <sstream>
#include <unordered_map>

#include "parser/parse_utils.hpp"
#include "scenario/scenario_impl.hpp"
#include "utils/identifier_factory.hpp"
#include "logger/logger.hpp"

namespace sim {

// shared utils from parser
static std::vector<std::string> split_ids_by_comma(const std::string& s) {
    std::vector<std::string> out;
    std::string token;
    std::stringstream ss(s);
    while (std::getline(ss, token, ',')) {
        token.erase(token.begin(), std::find_if(token.begin(), token.end(),
                                                [](unsigned char ch) { return !std::isspace(ch); }));
        token.erase(std::find_if(token.rbegin(), token.rend(),
                                 [](unsigned char ch) { return !std::isspace(ch); }).base(),
                    token.end());
        if (!token.empty()) out.emplace_back(std::move(token));
    }
    return out;
}

static std::vector<std::string> parse_connection_ids(
    const YAML::Node& node,
    const std::vector<std::string>& all_conn_ids)
{
    std::vector<std::string> ids;

    if (node.IsScalar()) {
        auto s = node.as<std::string>();
        if (s.find(',') != std::string::npos) {
            ids = split_ids_by_comma(s);
        } else {
            try {
                std::regex re(s);
                for (const auto& id : all_conn_ids) {
                    if (std::regex_match(id, re)) {
                        ids.push_back(id);
                    }
                }
            } catch (const std::regex_error& e) {
                throw std::runtime_error(fmt::format(
                    "Invalid regex in `connections`: '{}': {}", s, e.what()));
            }
        }
    } else if (node.IsSequence()) {
        ids.reserve(node.size());
        for (const auto& it : node) {
            ids.emplace_back(it.as<std::string>());
        }
    } else {
        throw std::runtime_error("`connections` must be scalar or sequence");
    }

    if (ids.empty()) {
        throw std::runtime_error("No connections matched for `connections` field");
    }

    return ids;
}

void SendDataConnHandler::handle(const YAML::Node& node, ScenarioImpl& scenario) {
    if (!node["when"]) throw std::runtime_error("send_data_conn requires `when` field");
    if (!node["size"]) throw std::runtime_error("send_data_conn requires `size` field");
    if (!node["connections"]) throw std::runtime_error("send_data_conn requires `connections` field");

    const TimeNs when = parse_time(node["when"].as<std::string>());
    const SizeByte size = parse_size(node["size"].as<std::string>());

    int num = 1;
    if (auto n = node["num"]; n) {
        num = n.as<int>();
        if (num <= 0) throw std::runtime_error("`num` must be > 0");
    }

    TimeNs gap(0);
    if (auto g = node["gap"]; g) {
        gap = parse_time(g.as<std::string>());
    }

    auto& factory = IdentifierFactory::get_instance();
    const auto all_conn_ids = factory.get_all_ids<IConnection>();
    const auto ids = parse_connection_ids(node["connections"], all_conn_ids);

    std::unordered_map<std::string, std::shared_ptr<IConnection>> conn_index;
    conn_index.reserve(all_conn_ids.size());
    for (const auto& id : all_conn_ids) {
        auto ptr = factory.get_object<IConnection>(id);
        if (ptr) conn_index.emplace(id, std::move(ptr));
    }

    ScenarioImpl::SendDataConnBatch batch;
    batch.when = when;
    batch.size = size;
    batch.num = num;
    batch.gap = gap;

    for (const auto& id : ids) {
        auto it = conn_index.find(id);
        if (it == conn_index.end() || !it->second) {
            throw std::runtime_error(fmt::format(
                "Scenario references unknown connection id '{}'", id));
        }
        batch.conns.emplace_back(it->second);
    }

    scenario.add_send_data_conn_batch(std::move(batch));
}

}  // namespace sim
