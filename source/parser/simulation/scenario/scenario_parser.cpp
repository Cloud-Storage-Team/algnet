#include "parser/simulation/scenario/scenario_parser.hpp"

#include <algorithm>
#include <regex>
#include <sstream>
#include <unordered_map>

#include "logger/logger.hpp"
#include "parser/parse_utils.hpp"
#include "scenario/scenario_impl.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

// Split a string of the form "conn1, conn2, conn3"
static std::vector<std::string> split_ids_by_comma(const std::string& s) {
    std::vector<std::string> out;
    std::string token;
    std::stringstream ss(s);
    while (std::getline(ss, token, ',')) {
        // trim
        token.erase(token.begin(), std::find_if(token.begin(), token.end(),
                                                [](unsigned char ch) { return !std::isspace(ch); }));
        token.erase(std::find_if(token.rbegin(), token.rend(),
                                 [](unsigned char ch) { return !std::isspace(ch); }).base(),
                    token.end());
        if (!token.empty()) out.emplace_back(std::move(token));
    }
    return out;
}

std::shared_ptr<IScenario> ScenarioParser::parse(const YAML::Node& scenario_node) {
    if (!scenario_node || !scenario_node.IsSequence()) {
        throw std::runtime_error("`scenario` must be a sequence");
    }

    auto scenario = std::make_shared<ScenarioImpl>();

    // prepare connection index for quick lookup
    auto& factory = IdentifierFactory::get_instance();
    const auto all_conn_ids = factory.get_all_ids<IConnection>();

    std::unordered_map<std::string, std::shared_ptr<IConnection>> conn_index;
    conn_index.reserve(all_conn_ids.size());
    for (const auto& id : all_conn_ids) {
        auto ptr = factory.get_object<IConnection>(id);
        if (ptr) conn_index.emplace(id, std::move(ptr));
    }

    for (const auto& node : scenario_node) {
        const auto action = node["action"] ? node["action"].as<std::string>() : std::string{};
        if (action.empty()) {
            throw std::runtime_error("Scenario item must contain `action`");
        }

        // add more action types here as needed
        if (action == "send_data_conn") {
            // strictly required fields
            if (!node["when"]) throw std::runtime_error("send_data_conn requires `when` field");
            if (!node["size"]) throw std::runtime_error("send_data_conn requires `size` field");

            const TimeNs   when = parse_time(node["when"].as<std::string>());
            const SizeByte size = parse_size(node["size"].as<std::string>());

            // optional
            int num = 1;
            if (auto n = node["num"]; n) {
                num = n.as<int>();
                if (num <= 0) throw std::runtime_error("`num` must be > 0");
            }

            TimeNs gap(0);
            if (auto g = node["gap"]; g) {
                gap = parse_time(g.as<std::string>());
            }

            if (!node["connections"]) {
                throw std::runtime_error("send_data_conn requires `connections` field");
            }

            // parse connections field: either comma-separated list or regex
            std::vector<std::string> ids;
            const auto cs = node["connections"];

            if (cs.IsScalar()) {
                auto s = cs.as<std::string>();
                if (s.find(',') != std::string::npos) {
                    ids = split_ids_by_comma(s);
                } else {
                    // treat as regex
                    std::regex re(s);
                    for (const auto& id : all_conn_ids) {
                        if (std::regex_match(id, re)) {
                            ids.push_back(id);
                        }
                    }
                }
            } else if (cs.IsSequence()) {
                ids.reserve(cs.size());
                for (const auto& it : cs) ids.emplace_back(it.as<std::string>());
            } else {
                throw std::runtime_error("`connections` must be scalar or sequence");
            }

            if (ids.empty()) {
                throw std::runtime_error("No connections matched for send_data_conn");
            }

            // build batch and add to scenario
            ScenarioImpl::SendDataConnBatch batch;
            batch.when = when;
            batch.size = size;
            batch.num  = num;
            batch.gap  = gap;

            batch.conns.reserve(ids.size());
            for (const auto& id : ids) {
                auto it = conn_index.find(id);
                if (it == conn_index.end() || !it->second) {
                    throw std::runtime_error(fmt::format(
                        "Scenario references unknown connection id '{}'", id));
                }
                batch.conns.emplace_back(it->second);
            }

            scenario->add_send_data_conn_batch(std::move(batch));

        } else {
            LOG_WARN(fmt::format("Unknown scenario action '{}'; skipped", action));
        }
    }

    return scenario;
}

}  // namespace sim
