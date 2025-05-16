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

    void processDevices(const YAML::Node& config, SimulatorVariant& simulator);
    void processLinks(const YAML::Node& config,
                       SimulatorVariant& simulator) const;
    void processFlows(const YAML::Node& config,
                       SimulatorVariant& simulator) const;

    std::filesystem::path parseTopologyConfigPath(const YAML::Node& config);
    std::string parseAlgorithm(const YAML::Node& config) const;
    void parseSimulationTime(const YAML::Node& config);

    std::map<std::string, std::shared_ptr<IRoutingDevice>> m_devices;
    std::filesystem::path m_topology_config_path;
    Time m_simulation_time = 0;
};

}  // namespace sim
