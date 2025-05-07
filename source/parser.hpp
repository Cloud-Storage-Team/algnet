#pragma once

#include <yaml-cpp/yaml.h>

#include <cstdint>
#include <filesystem>
#include <map>
#include <string>

#include "simulator.hpp"

namespace sim {

class YamlParser {
public:
    SimulatorVariant buildSimulatorFromConfig(
        const std::filesystem::path& path);
    Time get_simulation_time() const;

private:
    std::map<std::string, std::shared_ptr<IRoutingDevice>> m_devices;
    std::filesystem::path m_topology_config_path;
    Time m_simulation_time = 0;

    static uint32_t parse_throughput(const std::string& throughput_str);
    static uint32_t parse_latency(const std::string& latency_str);

    void process_hosts(const YAML::Node& config, SimulatorVariant& simulator);
    void process_switches(const YAML::Node& config,
                          SimulatorVariant& simulator);
    void process_links(const YAML::Node& config,
                       SimulatorVariant& simulator) const;
    void process_flows(const YAML::Node& config,
                       SimulatorVariant& simulator) const;

    std::filesystem::path parse_topology_config_path(const YAML::Node& config);
    std::string parse_algorithm(const YAML::Node& config) const;
    void parse_simulation_time(const YAML::Node& config);
};

}  // namespace sim
