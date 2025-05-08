#include "parser.hpp"

#include <yaml-cpp/yaml.h>

#include <stdexcept>
#include <string>

#include "logger/logger.hpp"
#include "simulator.hpp"

namespace sim {

SimulatorVariant YamlParser::buildSimulatorFromConfig(
    const std::filesystem::path &path) {
    const YAML::Node simulation_config = YAML::LoadFile(path);
    std::string algorithm = parse_algorithm(simulation_config);
    SimulatorVariant simulator = create_simulator(algorithm);

    m_topology_config_path =
        path.parent_path() / parse_topology_config_path(simulation_config);
    const YAML::Node topology_config = YAML::LoadFile(m_topology_config_path);

    process_hosts(topology_config, simulator);
    process_switches(topology_config, simulator);
    process_links(topology_config, simulator);

    process_flows(simulation_config, simulator);
    parse_simulation_time(simulation_config);
    m_devices.clear();
    return simulator;
}

void YamlParser::parse_simulation_time(const YAML::Node &config) {
    if (!config["simulation_time"]) {
        throw std::runtime_error("No simulation time specified in the simulation config");
    }
    m_simulation_time = config["simulation_time"].as<Time>();
}

uint32_t YamlParser::parse_throughput(const std::string &throughput_str) {
    const size_t unit_pos = throughput_str.find_first_not_of("0123456789");
    if (unit_pos == std::string::npos) {
        throw std::runtime_error("Invalid throughput: " + throughput_str);
    }
    const uint32_t value = std::stoul(throughput_str.substr(0, unit_pos));
    const std::string unit = throughput_str.substr(unit_pos);
    if (unit == "Gbps") {
        return value * 1000;  // Convert to Mbps
    }
    if (unit == "Mbps") {
        return value;
    }
    throw std::runtime_error("Unsupported throughput unit: " + unit);
}

uint32_t YamlParser::parse_latency(const std::string &latency_str) {
    const size_t unit_pos = latency_str.find_first_not_of("0123456789");
    if (unit_pos == std::string::npos) {
        throw std::runtime_error("Invalid latency: " + latency_str);
    }
    const uint32_t value = std::stoul(latency_str.substr(0, unit_pos));
    const std::string unit = latency_str.substr(unit_pos);
    if (unit == "ns") {
        return value;
    }
    throw std::runtime_error("Unsupported latency unit: " + unit);
}

void YamlParser::process_hosts(const YAML::Node &config,
                               SimulatorVariant &simulator) {
    if (!config["hosts"]) {
        LOG_WARN("No hosts specified in the topology config");
        return;
    }
    for (auto it = config["hosts"].begin(); it != config["hosts"].end(); ++it) {
        const YAML::Node key_node = it->first;
        const YAML::Node val_node = it->second;

        auto key = key_node.as<std::string>();
        auto type_str = val_node["type"].as<std::string>();
        const auto name = val_node["name"].as<std::string>();

        if (type_str == "sender") {
            m_devices[key] = std::visit(
                [&name](auto &sim) { return sim.add_sender(name); }, simulator);
        } else if (type_str == "receiver") {
            m_devices[key] = std::visit(
                [&name](auto &sim) { return sim.add_receiver(name); },
                simulator);

        } else {
            throw std::runtime_error("Invalid host type: " + type_str);
        }
    }
}

void YamlParser::process_switches(const YAML::Node &config,
                                  SimulatorVariant &simulator) {
    if (!config["switches"]) {
        LOG_WARN("No switches specified in the topology config");
        return;
    }

    const YAML::Node switches = config["switches"];
    for (auto it = switches.begin(); it != switches.end(); ++it) {
        auto key = it->first.as<std::string>();
        const auto name = it->second["name"].as<std::string>();
        m_devices[key] = std::visit(
            [&name](auto &sim) { return sim.add_switch(name); }, simulator);
    }
}

void YamlParser::process_links(const YAML::Node &config,
                               SimulatorVariant &simulator) const {
    if (!config["links"]) {
        LOG_WARN("No links specified in the topology config");
        return;
    }

    const YAML::Node links = config["links"];
    for (auto it = links.begin(); it != links.end(); ++it) {
        const YAML::Node link = it->second;
        auto from = link["from"].as<std::string>();
        auto to = link["to"].as<std::string>();
        const uint32_t latency =
            parse_latency(link["latency"].as<std::string>());
        const uint32_t speed =
            parse_throughput(link["throughput"].as<std::string>());

        auto from_it = m_devices.find(from);
        auto to_it = m_devices.find(to);
        if (from_it == m_devices.end()) {
            throw std::runtime_error("Unknown device in 'from': " + from);
        }
        if (to_it == m_devices.end()) {
            throw std::runtime_error("Unknown device in 'to': " + to);
        }

        const auto from_dev = from_it->second;
        if (const auto to_dev = to_it->second; !from_dev || !to_dev) {
            throw std::runtime_error("Device pointer is null for " + from +
                                     " or " + to);
        }

        std::visit(
            [&](auto &sim) {
                sim.add_link(m_devices.at(from), m_devices.at(to), speed,
                             latency);
            },
            simulator);
    }
}

void YamlParser::process_flows(const YAML::Node &config,
                               SimulatorVariant &simulator) const {
    if (!config["flows"]) {
        LOG_ERROR("No flows specified in the simulation config");
        return;
    }

    const YAML::Node &flows = config["flows"];
    for (auto it = flows.begin(); it != flows.end(); ++it) {
        std::string key = it->first.as<std::string>();
        std::string sender_id = it->second["sender_id"].as<std::string>();
        if (!m_devices.contains(sender_id)) {
            throw std::runtime_error("Unknown device in 'sender_id': " + sender_id);
        }
        std::string receiver_id = it->second["receiver_id"].as<std::string>();
        if (!m_devices.contains(receiver_id)) {
            throw std::runtime_error("Unknown device in 'receiver_id': " + receiver_id);
        }

        Size packet_size = it->second["packet_size"].as<Size>();
        Time packet_interval = it->second["packet_interval"].as<Time>();

        std::shared_ptr<ISender> sender =
            std::dynamic_pointer_cast<ISender>(m_devices.at(sender_id));
        std::shared_ptr<IReceiver> receiver =
            std::dynamic_pointer_cast<IReceiver>(m_devices.at(receiver_id));

        std::visit(
            [&](auto &sim) {
                sim.add_flow(sender, receiver, packet_size, packet_interval, 0);
            },
            simulator);
    }
}

std::filesystem::path YamlParser::parse_topology_config_path(
    const YAML::Node &config) {
    if (!config["topology_config_path"]) {
        throw std::runtime_error("No topology_config_path specified in the simulation config");
    }
    return config["topology_config_path"].as<std::string>();
}

std::string YamlParser::parse_algorithm(const YAML::Node &config) const {
    if (!config["algorithm"]) {
        throw std::runtime_error("No algorithm specified in the simulation config");
    }
    return config["algorithm"].as<std::string>();
}

Time YamlParser::get_simulation_time() const {
    if (m_simulation_time == 0) {
        LOG_ERROR("Simulation time is 0");
    }
    return m_simulation_time;
}

}  // namespace sim
