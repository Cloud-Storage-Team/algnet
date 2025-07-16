#include "parser/parser.hpp"

#include <stdexcept>

#include "identifiable_parser/flow/parse_tcp_flow.hpp"
#include "identifiable_parser/identifiable_parser.hpp"
#include "logger/logger.hpp"

namespace sim {

std::pair<SimulatorVariant, Time> YamlParser::build_simulator_from_config(
    const std::filesystem::path &path) {
    const YAML::Node simulation_config = YAML::LoadFile(path);
    std::string algorithm = parse_algorithm(simulation_config);
    m_simulator = create_simulator(algorithm);

    m_topology_config_path =
        path.parent_path() / parse_topology_config_path(simulation_config);
    const YAML::Node topology_config = YAML::LoadFile(m_topology_config_path);

    process_devices(topology_config);
    process_links(topology_config);

    process_flows(simulation_config);
    return {m_simulator, parse_simulation_time(simulation_config)};
}

Time YamlParser::parse_simulation_time(const YAML::Node &config) {
    if (!config["simulation_time"]) {
        throw std::runtime_error(
            "No simulation time specified in the simulation config");
    }
    return config["simulation_time"].as<Time>();
}

void YamlParser::process_devices(const YAML::Node &config) {
    if (!config["devices"]) {
        LOG_WARN("No devices specified in the topology config");
        return;
    }
    for (auto it = config["devices"].begin(); it != config["devices"].end();
         ++it) {
        const YAML::Node key_node = it->first;
        const YAML::Node val_node = it->second;

        auto device_name = key_node.as<Id>();
        auto device_type = val_node["type"].as<std::string>();

        if (device_type == "host") {
            std::visit(
                [&key_node, &val_node](auto &sim) {
                    using SimType = std::decay_t<decltype(sim)>;
                    using HostType = typename SimType::Host_T;
                    std::shared_ptr<HostType> ptr =
                        IdentifieableParser<HostType>::parse_and_registrate(
                            key_node, val_node);
                    if (!sim.add_host(ptr)) {
                        throw std::runtime_error("Can not add host with id " +
                                                 ptr.get()->get_id());
                    }
                },
                m_simulator);
        } else if (device_type == "switch") {
            std::visit(
                [&key_node, &val_node](auto &simulator) {
                    using SimType = std::decay_t<decltype(simulator)>;
                    using SwitchType = typename SimType::Switch_T;
                    std::shared_ptr<SwitchType> ptr =
                        IdentifieableParser<SwitchType>::parse_and_registrate(
                            key_node, val_node);
                    if (!simulator.add_switch(ptr)) {
                        throw std::runtime_error("Can not add switch with id " +
                                                 ptr.get()->get_id());
                    }
                },
                m_simulator);
        } else {
            throw std::runtime_error("Invalid host type: " + device_type);
        }
    }
}

void YamlParser::process_links(const YAML::Node &config) {
    if (!config["links"]) {
        LOG_WARN("No links specified in the topology config");
        return;
    }

    const YAML::Node links = config["links"];
    for (auto it = links.begin(); it != links.end(); ++it) {
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

void YamlParser::process_flows(const YAML::Node &config) {
    if (!config["flows"]) {
        LOG_ERROR("No flows specified in the simulation config");
        return;
    }

    const YAML::Node &flows = config["flows"];
    for (auto it = flows.begin(); it != flows.end(); ++it) {
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
