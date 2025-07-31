#include "parser/parser.hpp"

#include <stdexcept>

#include "identifiable_parser/flow/parse_tcp_flow.hpp"
#include "identifiable_parser/identifiable_parser.hpp"
#include "logger/logger.hpp"

namespace sim {

std::pair<SimulatorVariant, TimeNs> YamlParser::build_simulator_from_config(
    const std::filesystem::path &path) {
    const YAML::Node simulation_config = YAML::LoadFile(path);
    std::string algorithm = parse_algorithm(simulation_config);
    m_simulator = create_simulator(algorithm);

    m_topology_config_path =
        path.parent_path() / parse_topology_config_path(simulation_config);
    const YAML::Node topology_config = YAML::LoadFile(m_topology_config_path);

    auto parse_if_present = [](const YAML::Node &node,
                               std::function<void(const YAML::Node &)> parser,
                               std::string error_message) {
        if (node) {
            parser(node);
        } else {
            LOG_ERROR(std::move(error_message));
        }
    };

    parse_if_present(
        topology_config["hosts"],
        [this](auto node) { return process_hosts(node); },
        "No hosts specified in the topology config");

    parse_if_present(
        topology_config["switches"],
        [this](auto node) { return process_switches(node); },
        "No switches specified in the topology config");

    parse_if_present(
        topology_config["links"],
        [this](auto node) { return process_links(node); },
        "No links specified in the topology config");

    parse_if_present(
        simulation_config["flows"],
        [this](auto node) { return process_flows(node); },
        "No flows specified in the simulation config");

    return {m_simulator, parse_simulation_time(simulation_config)};
}

TimeNs YamlParser::parse_simulation_time(const YAML::Node &config) {
    if (!config["simulation_time"]) {
        throw std::runtime_error(
            "No simulation time specified in the simulation config");
    }
    return TimeNs(config["simulation_time"].as<uint32_t>());
}

void YamlParser::process_hosts(const YAML::Node &hosts_node) {
    for (auto it = hosts_node.begin(); it != hosts_node.end(); ++it) {
        const YAML::Node key_node = it->first;
        const YAML::Node val_node = it->second;

        auto device_name = key_node.as<Id>();
        std::visit(
            [&key_node, &val_node](auto &sim) {
                std::shared_ptr<Host> ptr =
                    IdentifieableParser<Host>::parse_and_registrate(key_node,
                                                                    val_node);
                if (!sim.add_host(ptr)) {
                    throw std::runtime_error("Can not add host with id " +
                                             ptr.get()->get_id());
                }
            },
            m_simulator);
    }
}

void YamlParser::process_switches(const YAML::Node &swtiches_node) {
    for (auto it = swtiches_node.begin(); it != swtiches_node.end(); ++it) {
        const YAML::Node key_node = it->first;
        const YAML::Node val_node = it->second;

        std::visit(
            [&key_node, &val_node](auto &simulator) {
                std::shared_ptr<Switch> ptr =
                    IdentifieableParser<Switch>::parse_and_registrate(key_node,
                                                                      val_node);
                if (!simulator.add_switch(ptr)) {
                    throw std::runtime_error("Can not add switch with id " +
                                             ptr.get()->get_id());
                }
            },
            m_simulator);
    }
}

void YamlParser::process_links(const YAML::Node &links_node) {
    for (auto it = links_node.begin(); it != links_node.end(); ++it) {
        const YAML::Node key_node = it->first;
        const YAML::Node value_node = it->second;
        std::visit(
            [&](auto &sim) {
                using SimType = std::decay_t<decltype(sim)>;
                using LinkType = typename SimType::Link_T;
                std::shared_ptr<LinkType> ptr =
                    IdentifieableParser<LinkType>::parse_and_registrate(
                        key_node, value_node);
                if (!sim.add_link(ptr)) {
                    throw std::runtime_error("Can not add link with id " +
                                             ptr.get()->get_id());
                }
            },
            m_simulator);
    }
}

void YamlParser::process_flows(const YAML::Node &flows_node) {
    for (auto it = flows_node.begin(); it != flows_node.end(); ++it) {
        const YAML::Node key_node = it->first;
        const YAML::Node val_node = it->second;
        std::visit(
            [&](auto &sim) {
                using SimType = std::decay_t<decltype(sim)>;
                using FlowType = typename SimType::Flow_T;
                std::shared_ptr<FlowType> ptr =
                    IdentifieableParser<FlowType>::parse_and_registrate(
                        key_node, val_node);
                if (!sim.add_flow(ptr)) {
                    throw std::runtime_error("can not add flow with id " +
                                             ptr.get()->get_id());
                }
            },
            m_simulator);
    }
}

std::filesystem::path YamlParser::parse_topology_config_path(
    const YAML::Node &config) {
    if (!config["topology_config_path"]) {
        throw std::runtime_error(
            "No topology_config_path specified in the simulation config");
    }
    return config["topology_config_path"].as<std::string>();
}

std::string YamlParser::parse_algorithm(const YAML::Node &config) {
    if (!config["algorithm"]) {
        throw std::runtime_error(
            "No algorithm specified in the simulation config");
    }
    return config["algorithm"].as<std::string>();
}

}  // namespace sim
