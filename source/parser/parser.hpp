#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <string>
#include <utility>

#include "simulator.hpp"

namespace sim {

class YamlParser {
public:
    std::pair<SimulatorVariant, TimeNs> build_simulator_from_config(
        const std::filesystem::path& path);

private:
    static std::filesystem::path parse_topology_config_path(
        const YAML::Node& config);
    static std::string parse_algorithm(const YAML::Node& config);
    static TimeNs parse_simulation_time(const YAML::Node& config);

    void process_hosts(const YAML::Node& hosts_node);
    void process_switches(const YAML::Node& switches_node);

    void process_links(const YAML::Node& links_node);
    void process_connections(const YAML::Node& connections_node);
    void process_flows(const YAML::Node& flows_node);

    SimulatorVariant m_simulator;
    std::filesystem::path m_topology_config_path;
};

}  // namespace sim
