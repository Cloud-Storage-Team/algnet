#include "parser/simulation/scenario/scenario_parser.hpp"

#include <memory>
#include <unordered_map>

#include "handlers/send_data_conn_handler.hpp"
#include "logger/logger.hpp"
#include "scenario/scenario_impl.hpp"

namespace sim {

std::shared_ptr<IScenario> ScenarioParser::parse(
    const YAML::Node& scenario_node) {
    if (!scenario_node || !scenario_node.IsSequence()) {
        throw std::runtime_error("`scenario` must be a sequence");
    }

    auto scenario = std::make_shared<ScenarioImpl>();

    std::unordered_map<std::string, std::unique_ptr<IScenarioActionHandler>>
        handlers;

    handlers.emplace("send_data_conn", std::make_unique<SendDataConnHandler>());

    for (const auto& node : scenario_node) {
        const auto action =
            node["action"] ? node["action"].as<std::string>() : std::string{};
        if (action.empty()) {
            throw std::runtime_error("Scenario item must contain `action`");
        }

        auto it = handlers.find(action);
        if (it != handlers.end()) {
            it->second->handle(node, *scenario);
        } else {
            throw std::runtime_error(
                fmt::format("Unknown scenario action '{}'", action));
        }
    }

    return scenario;
}

}  // namespace sim
