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
        path.parent_path() / simulation_config["topology_config_path"]
                                 .value_or_throw()
                                 .as<std::string>()
                                 .value_or_throw();
    const ConfigNode topology_config = load_file(m_topology_config_path);

    auto parse_if_present = [](ConfigNodeExpected node,
                               std::function<void(ConfigNode)> parser) {
        node.apply_or(parser,
                      [](std::string error) { LOG_ERROR(std::move(error)); });
    };

    ConfigNodeExpected topology_presets_node = topology_config["presets"];

    const ConfigNode packet_spraying_node =
        topology_config["packet-spraying"].value_or_throw();
    parse_if_present(topology_config["hosts"],
                     [this](ConfigNode node) { process_hosts(node); });

    parse_if_present(topology_config["switches"],
                     [this, &packet_spraying_node](ConfigNode node) {
                         return process_switches(node, packet_spraying_node);
                     });

    ConfigNode links_preset_node =
        topology_presets_node.value_or(ConfigNode())["link"].value_or(
            ConfigNode());
    parse_if_present(topology_config["links"],
                     [this, &links_preset_node](ConfigNode node) {
                         process_links(node, links_preset_node);
                     });

    parse_if_present(simulation_config["connections"],
                     [this](ConfigNode node) { process_connection(node); });

    parse_if_present(simulation_config["scenario"],
                     [this](auto node) { return process_scenario(node); });

    return std::move(m_simulator);
}

void YamlParser::process_hosts(const ConfigNode &hosts_node) {
    process_identifiables<IHost>(
        hosts_node,
        [this](std::shared_ptr<IHost> host) {
            return m_simulator.add_host(host);
        },
        [](const ConfigNode& hosts_node){
            return HostParser::parse_i_host(ConfigNodeWithPreset(hosts_node));
        },
        "Can not add host.");
}

void YamlParser::process_switches(const ConfigNode &switches_node,
                                  const ConfigNode &packet_spraying_node) {
    process_identifiables<ISwitch>(
        switches_node,
        [this](std::shared_ptr<ISwitch> swtch) {
            return m_simulator.add_switch(swtch);
        },
        [&packet_spraying_node](const ConfigNode &switch_node) {
            return SwitchParser::parse_i_switch(ConfigNodeWithPreset(switch_node),
                                                ConfigNodeWithPreset(packet_spraying_node));
        },
        "Can not add switch.");
}

void YamlParser::process_links(const ConfigNode &links_node,
                               const ConfigNode &link_presets_node) {
    process_identifiables<ILink>(
        links_node,
        [this](std::shared_ptr<ILink> link) {
            return m_simulator.add_link(link);
        },
        [&link_presets_node](const ConfigNode &link_node) {
            return LinkParser::parse_i_link(ConfigNodeWithPreset(link_node, link_presets_node));
        },
        "Can not add link.");
}

void YamlParser::process_connection(const ConfigNode &connections_node) {
    process_identifiables<IConnection>(
        connections_node,
        [this](std::shared_ptr<IConnection> connection) {
            return m_simulator.add_connection(connection);
        },
        ConnectionParser::parse_i_connection, "Can not add connection.",
        RegistrationPolicy::ByParser);
}

void YamlParser::process_scenario(const ConfigNode &scenario_node) {
    auto scenario = ScenarioParser::parse(scenario_node);
    m_simulator.set_scenario(std::move(scenario));
}

}  // namespace sim
