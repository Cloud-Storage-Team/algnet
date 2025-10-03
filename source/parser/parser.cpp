#include "parser/parser.hpp"

#include <stdexcept>

#include "logger/logger.hpp"
#include "parser/simulation/connection/connection_parser.hpp"
#include "parser/simulation/scenario/scenario_parser.hpp"
#include "parser/topology/host/host_parser.hpp"
#include "parser/topology/link/link_parser.hpp"
#include "parser/topology/switch/switch_parser.hpp"
#include "preset_storage.hpp"

namespace sim {

Simulator YamlParser::build_simulator_from_config(
    const std::filesystem::path &path) {
    const YAML::Node simulation_config = YAML::LoadFile(path);

    m_simulator = Simulator();

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

    YAML::Node topology_presets_node = topology_config["presets"];

    const YAML::Node packet_spraying_node = topology_config["packet-spraying"];
    if (!packet_spraying_node) {
        throw std::runtime_error(
            "Packet spraying type does not set in topology config");
    }
    parse_if_present(
        topology_config["hosts"],
        [this](auto node) { return process_hosts(node); },
        "No hosts specified in the topology config");

    parse_if_present(
        topology_config["switches"],
        [this, &packet_spraying_node](auto node) {
            return process_switches(node, packet_spraying_node);
        },
        "No switches specified in the topology config");

    parse_if_present(
        topology_config["links"],
        [this, &topology_presets_node](auto node) {
            return process_links(node,
                                 get_if_present(topology_presets_node, "link"));
        },
        "No links specified in the topology config");

    parse_if_present(
        simulation_config["connections"],
        [this](auto node) { return process_connection(node); },
        "No connections specified in the simulation config");

    parse_if_present(
        simulation_config["scenario"],
        [this](auto node) { return process_scenario(node); },
        "No scenario specified in the simulation config");

    std::optional<TimeNs> maybe_stop_time =
        parse_simulation_time(simulation_config);
    if (maybe_stop_time.has_value()) {
        m_simulator.set_stop_time(maybe_stop_time.value());
    }

    return std::move(m_simulator);
}

std::optional<TimeNs> YamlParser::parse_simulation_time(
    const YAML::Node &config) {
    auto value = config["simulation_time"];
    if (!value) {
        return std::nullopt;
    }
    return parse_time(value.as<std::string>());
}

void YamlParser::process_hosts(const YAML::Node &hosts_node) {
    process_identifiables<IHost>(
        hosts_node,
        [this](std::shared_ptr<IHost> host) {
            return m_simulator.add_host(host);
        },
        HostParser::parse_i_host, "Can not add host.");
}

void YamlParser::process_switches(const YAML::Node &swtiches_node,
                                  const YAML::Node &packet_spraying_node) {
    process_identifiables<ISwitch>(
        swtiches_node,
        [this](std::shared_ptr<ISwitch> swtch) {
            return m_simulator.add_switch(swtch);
        },
        [&packet_spraying_node](const YAML::Node &key_node,
                                const YAML::Node &value_node) {
            return SwitchParser::parse_i_switch(key_node, value_node,
                                                packet_spraying_node);
        },
        "Can not add switch.");
}

void YamlParser::process_links(const YAML::Node &links_node,
                               const YAML::Node &link_presets_node) {
    // Maps preset name to preset body
    LinkPresets presets(link_presets_node, [](const YAML::Node &preset_node) {
        LinkInitArgs args;
        LinkParser::parse_to_args(preset_node, args);
        return args;
    });
    process_identifiables<ILink>(
        links_node,
        [this](std::shared_ptr<ILink> link) {
            return m_simulator.add_link(link);
        },
        [&presets](const YAML::Node &key_node, const YAML::Node &value_node) {
            return LinkParser::parse_i_link(key_node, value_node, presets);
        },
        "Can not add link.");
}

void YamlParser::process_connection(const YAML::Node &connections_node) {
    process_identifiables<IConnection>(
        connections_node,
        [this](std::shared_ptr<IConnection> connection) {
            return m_simulator.add_connection(connection);
        },
        ConnectionParser::parse_i_connection, "Can not add connection.",
        RegistrationPolicy::ByParser);
}

void YamlParser::process_scenario(const YAML::Node &scenario_node) {
    auto scenario = ScenarioParser::parse(scenario_node);
    m_simulator.set_scenario(std::move(scenario));
}

std::filesystem::path YamlParser::parse_topology_config_path(
    const YAML::Node &config) {
    if (!config["topology_config_path"]) {
        throw std::runtime_error(
            "No topology_config_path specified in the simulation config");
    }
    return config["topology_config_path"].as<std::string>();
}

}  // namespace sim
