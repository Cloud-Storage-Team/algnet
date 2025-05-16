#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <map>
#include <string>
#include <utility>

#include "simulator.hpp"

namespace sim {

class YamlParser {
public:
    std::pair<SimulatorVariant, Time> buildSimulatorFromConfig(
        const std::filesystem::path& path);

private:
    static uint32_t parseThroughput(const std::string& throughput_str);
    static uint32_t parseLatency(const std::string& latency_str);

    static std::filesystem::path parseTopologyConfigPath(
        const YAML::Node& config);
    static std::string parseAlgorithm(const YAML::Node& config);
    static Time parseSimulationTime(const YAML::Node& config);

    void processDevices(const YAML::Node& config);
    void processLinks(const YAML::Node& config);
    void processFlows(const YAML::Node& config);

    SimulatorVariant m_simulator;
    std::map<std::string, std::shared_ptr<IRoutingDevice>> m_devices;
    std::filesystem::path m_topology_config_path;
};

}  // namespace sim
