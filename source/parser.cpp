#include "parser.hpp"

#include <yaml-cpp/yaml.h>

#include <stdexcept>
#include <string>

#include "logger/logger.hpp"

namespace sim {

BasicSimulator YamlParser::parseConfig(const std::string &filename) {
    BasicSimulator simulator;
    const YAML::Node config = YAML::LoadFile(filename);
    processHosts(config, simulator);
    processSwitches(config, simulator);
    processLinks(config, simulator);
    return simulator;
}

uint32_t YamlParser::parseThroughput(const std::string &throughput_str) {
    const size_t unit_pos = throughput_str.find_first_not_of("0123456789");
    if (unit_pos == std::string::npos) {
        throw std::runtime_error("Invalid throughput: " + throughput_str);
    }
    const uint32_t value = std::stoul(throughput_str.substr(0, unit_pos));
    const std::string unit = throughput_str.substr(unit_pos);
    if (unit == "Gbps") return value * 1000;  // Convert to Mbps
    if (unit == "Mbps") return value;
    throw std::runtime_error("Unsupported throughput unit: " + unit);
}

uint32_t YamlParser::parseLatency(const std::string &latency_str) {
    const size_t unit_pos = latency_str.find_first_not_of("0123456789");
    if (unit_pos == std::string::npos) {
        throw std::runtime_error("Invalid latency: " + latency_str);
    }
    const uint32_t value = std::stoul(latency_str.substr(0, unit_pos));
    const std::string unit = latency_str.substr(unit_pos);
    if (unit == "ns") return value;
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
            simulator.add_sender(name);
        } else if (type_str == "receiver") {
            simulator.add_receiver(name);
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
        simulator.add_switch(name);
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

        simulator.add_link(from, to, speed, latency);
    }
}

}  // namespace sim
