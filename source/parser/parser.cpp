#include "parser/parser.hpp"

#include <stdexcept>

#include "logger/logger.hpp"
#include "parse_utils.hpp"
#include "parser/simulation/flow/flow_parser.hpp"
#include "parser/topology/host/host_parser.hpp"
#include "parser/topology/link/link_parser.hpp"
#include "parser/topology/switch/switch_parser.hpp"

namespace sim {

std::pair<Simulator, TimeNs> YamlParser::build_simulator_from_config(
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
        [this, &topology_presets_node](auto node) {
            return process_links(node,
                                 get_if_present(topology_presets_node, "link"));
        },
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
    process_identifiables<IHost>(
        hosts_node,
        [this](std::shared_ptr<IHost> host) {
            return m_simulator.add_host(host);
        },
        HostParser::parse_i_host, "Can not add host.");
}

void YamlParser::process_switches(const YAML::Node &swtiches_node) {
    process_identifiables<ISwitch>(
        swtiches_node,
        [this](std::shared_ptr<ISwitch> swtch) {
            return m_simulator.add_switch(swtch);
        },
        SwitchParser::parse_i_switch, "Can not add switch.");
}

void YamlParser::process_links(
    const YAML::Node &links_node,
    [[maybe_unused]] const YAML::Node &link_preset_node) {
    process_identifiables<ILink>(
        links_node,
        [this](std::shared_ptr<ILink> link) {
            return m_simulator.add_link(link);
        },
        LinkParser::parse_i_link, "Can not add link.");
}

void YamlParser::process_flows(const YAML::Node &flows_node) {
    process_identifiables<IFlow>(
        flows_node,
        [this](std::shared_ptr<IFlow> flow) {
            return m_simulator.add_flow(flow);
        },
        FlowParser::parse_i_flow, "Can not add flow.");
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
