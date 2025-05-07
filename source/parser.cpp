#include "parser.hpp"

#include <yaml-cpp/yaml.h>

#include <stdexcept>
#include <string>

#include "logger/logger.hpp"
#include "simulator.hpp"

namespace sim {

BasicSimulator YamlParser::parseConfig(const std::string &filename) {
    BasicSimulator simulator;
    const YAML::Node simulation_config = YAML::LoadFile(filename);
    m_topology_config_path = parse_topology_config_path(simulation_config);
    const YAML::Node topology_config = YAML::LoadFile(m_topology_config_path);

    processHosts(topology_config, simulator);
    processSwitches(topology_config, simulator);
    processLinks(topology_config, simulator);

    process_flows(simulation_config, simulator);
    parse_simulation_time(simulation_config);
    m_devices_map.clear();
    return simulator;
}

std::string YamlParser::parse_topology_config_path(const YAML::Node &config) {
    if (!config["topology_config_path"]) {
        throw std::runtime_error("No topology_config_path provided");
    }
    return config["topology_config_path"].as<std::string>();
}

Time YamlParser::parse_simulation_time(const YAML::Node &config) {
    if (!config["simulation_time"]) {
        throw std::runtime_error("No simulation time provided");
    }
    return config["simulation_time"].as<Time>();
}

uint32_t YamlParser::parseThroughput(const std::string &throughput_str) {
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

uint32_t YamlParser::parseLatency(const std::string &latency_str) {
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

void YamlParser::processHosts(const YAML::Node &config,
                              BasicSimulator &simulator) {
    if (!config["hosts"]) {
        LOG_WARN("No hosts specified in the configuration");
        return;
    }
    for (auto it = config["hosts"].begin(); it != config["hosts"].end(); ++it) {
        const YAML::Node key_node = it->first;
        const YAML::Node val_node = it->second;

        auto key = key_node.as<std::string>();
        auto type_str = val_node["type"].as<std::string>();
        const auto name = val_node["name"].as<std::string>();

        if (type_str == "sender") {
            m_devices_map[key] = simulator.add_sender(name);
        } else if (type_str == "receiver") {
            m_devices_map[key] = simulator.add_receiver(name);
        } else {
            throw std::runtime_error("Invalid host type: " + type_str);
        }
    }
}

void YamlParser::processSwitches(const YAML::Node &config,
                                 BasicSimulator &simulator) {
    if (!config["switches"]) {
        LOG_WARN("No switches specified in the configuration");
        return;
    }

    const YAML::Node switches = config["switches"];
    for (auto it = switches.begin(); it != switches.end(); ++it) {
        auto key = it->first.as<std::string>();
        const auto name = it->second["name"].as<std::string>();
        m_devices_map[key] = simulator.add_switch(name);
    }
}

void YamlParser::processLinks(const YAML::Node &config,
                              BasicSimulator &simulator) const {
    if (!config["links"]) {
        LOG_WARN("No links specified in the configuration");
        return;
    }

    const YAML::Node links = config["links"];
    for (auto it = links.begin(); it != links.end(); ++it) {
        const YAML::Node link = it->second;
        auto from = link["from"].as<std::string>();
        auto to = link["to"].as<std::string>();
        const uint32_t latency =
            parseLatency(link["latency"].as<std::string>());
        const uint32_t speed =
            parseThroughput(link["throughput"].as<std::string>());

        auto from_it = m_devices_map.find(from);
        auto to_it = m_devices_map.find(to);
        if (from_it == m_devices_map.end()) {
            throw std::runtime_error("Unknown device in 'from': " + from);
        }
        if (to_it == m_devices_map.end()) {
            throw std::runtime_error("Unknown device in 'to': " + to);
        }

        const auto from_dev = from_it->second;
        if (const auto to_dev = to_it->second; !from_dev || !to_dev) {
            throw std::runtime_error("Device pointer is null for " + from +
                                     " or " + to);
        }

        simulator.add_link(m_devices_map.at(from), m_devices_map.at(to), speed,
                           latency);
    }
}

void YamlParser::process_flows(const YAML::Node &config,
                               BasicSimulator &simulator) const {
    if (!config["flows"]) {
        LOG_ERROR("No flows in simulation config");
        return;
    }

    const YAML::Node &flows = config["flows"];
    for (auto it = flows.begin(); it != flows.end(); ++it) {
        std::string key = it->first.as<std::string>();
        std::string sender_id = it->second["sender_id"].as<std::string>();
        std::string receiver_id = it->second["receiver_id"].as<std::string>();
        Size packet_size = it->second["packet_size"].as<Size>();
        Time packet_interval = it->second["packet_interval"].as<Time>();

        std::shared_ptr<ISender> sender =
            std::dynamic_pointer_cast<ISender>(m_devices_map.at(sender_id));
        std::shared_ptr<IReceiver> receiver =
            std::dynamic_pointer_cast<IReceiver>(m_devices_map.at(receiver_id));

        simulator.add_flow(sender, receiver, packet_size, packet_interval, 0);
    }
}

}  // namespace sim
