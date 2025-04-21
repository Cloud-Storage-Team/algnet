#pragma once

#include <yaml-cpp/node/node.h>

#include <cstdint>
#include <map>
#include <string>

#include "simulator.hpp"

namespace sim {

class YamlParser {
public:  // TODO: replace void with vector<SimulationInfo>
    void parse_configs(const std::string& topology_filename,
                       const std::string& simulation_filename,
                       Simulator& simulator);

private:
    std::map<std::string, std::shared_ptr<IRoutingDevice>> m_devices_map;

    static uint32_t parse_throughput(const std::string& throughput_str);
    static uint32_t parse_latency(const std::string& latency_str);

    void parse_topology_config(const std::string& filename,
                               Simulator& simulator);
    void parse_simulation_config(const std::string& filename,
                                 Simulator& simulator);

    void process_hosts(const YAML::Node& config, Simulator& simulator);
    void process_switches(const YAML::Node& config, Simulator& simulator);
    void process_links(const YAML::Node& config, Simulator& simulator) const;

    void process_flows(const YAML::Node& config, Simulator& simulator) const;
};

}  // namespace sim
