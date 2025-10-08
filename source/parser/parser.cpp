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
    const ConfigNode simulation_config = load_file(path);

    m_simulator = Simulator();

    m_topology_config_path =
        path.parent_path() /
        parse_topology_config_path(simulation_config.get_node());
    const ConfigNode topology_config = load_file(m_topology_config_path);

    auto parse_if_present = [](ConfigNodeExpected node,
                               std::function<void(ConfigNode)> parser) {
        node.apply_or(parser,
                      [](std::string error) { LOG_ERROR(std::move(error)); });
    };

    ConfigNodeExpected topology_presets_node = topology_config["presets"];

    const ConfigNode packet_spraying_node =
        topology_config["packet-spraying"].value_or_throw<std::runtime_error>();
    parse_if_present(topology_config["hosts"],
                     [this](ConfigNode node) { process_hosts(node); });

    parse_if_present(topology_config["switches"],
                     [this, &packet_spraying_node](ConfigNode node) {
                         return process_switches(node, packet_spraying_node);
                     });

    parse_if_present(topology_config["links"], [this, &topology_presets_node](
                                                   ConfigNode node) {
        ConfigNode links_preset_node =
            topology_presets_node.value_or(ConfigNode())["link"].value_or(
                ConfigNode());
        process_links(node, links_preset_node);
    });

    parse_if_present(simulation_config["connections"],
                     [this](ConfigNode node) { process_connection(node); });

    parse_if_present(simulation_config["scenario"],
                     [this](auto node) { return process_scenario(node); });

    std::optional<TimeNs> maybe_stop_time =
        parse_simulation_time(simulation_config.get_node());
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

void YamlParser::process_hosts(const ConfigNode &hosts_node) {
    process_identifiables<IHost>(
        hosts_node.get_node(),
        [this](std::shared_ptr<IHost> host) {
            return m_simulator.add_host(host);
        },
        HostParser::parse_i_host, "Can not add host.");
}

void YamlParser::process_switches(const ConfigNode &swtiches_node,
                                  const ConfigNode &packet_spraying_node) {
    process_identifiables<ISwitch>(
        swtiches_node.get_node(),
        [this](std::shared_ptr<ISwitch> swtch) {
            return m_simulator.add_switch(swtch);
        },
        [&packet_spraying_node](const YAML::Node &key_node,
                                const YAML::Node &value_node) {
            return SwitchParser::parse_i_switch(
                key_node, value_node, packet_spraying_node.get_node());
        },
        "Can not add switch.");
}

void YamlParser::process_links(const ConfigNode &links_node,
                               const ConfigNode &link_presets_node) {
    // Maps preset name to preset body
    LinkPresets presets(link_presets_node.get_node(),
                        [](const YAML::Node &preset_node) {
                            LinkInitArgs args;
                            LinkParser::parse_to_args(preset_node, args);
                            return args;
                        });
    process_identifiables<ILink>(
        links_node.get_node(),
        [this](std::shared_ptr<ILink> link) {
            return m_simulator.add_link(link);
        },
        [&presets](const YAML::Node &key_node, const YAML::Node &value_node) {
            return LinkParser::parse_i_link(key_node, value_node, presets);
        },
        "Can not add link.");
}

void YamlParser::process_connection(const ConfigNode &connections_node) {
    process_identifiables<IConnection>(
        connections_node.get_node(),
        [this](std::shared_ptr<IConnection> connection) {
            return m_simulator.add_connection(connection);
        },
        ConnectionParser::parse_i_connection, "Can not add connection.",
        RegistrationPolicy::ByParser);
}

void YamlParser::process_scenario(const ConfigNode &scenario_node) {
    auto scenario = ScenarioParser::parse(scenario_node.get_node());
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
