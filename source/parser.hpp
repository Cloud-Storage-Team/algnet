#pragma once

#include <yaml-cpp/yaml.h>

#include <cstdint>
#include <map>
#include <string>

#include "simulator.hpp"

namespace sim {
class YamlParser {
public:
    BasicSimulator buildSimulatorFromConfig(const std::string& filename);

private:
    std::map<std::string, std::shared_ptr<IRoutingDevice>> m_devices;
    std::string m_topology_config_path;


    static uint32_t parse_throughput(const std::string& throughput_str);
    static uint32_t parse_latency(const std::string& latency_str);

    void process_hosts(const YAML::Node& config, BasicSimulator& simulator);
    void process_switches(const YAML::Node& config, BasicSimulator& simulator);
    void process_links(const YAML::Node& config,
                      BasicSimulator& simulator) const;
    void process_flows(const YAML::Node& config,
                       BasicSimulator& simulator) const;

    void parse_simulation_config(const std::string& filename,
                                 BasicSimulator& simulator);
    std::string parse_topology_config_path(const YAML::Node& config);
    Time parse_simulation_time(const YAML::Node& config);


};
}  // namespace sim
