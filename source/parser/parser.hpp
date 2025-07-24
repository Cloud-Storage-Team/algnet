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
    static std::string parse_multipath_type(const YAML::Node& config);

    void process_devices(const YAML::Node& config);
    void process_links(const YAML::Node& config);
    void process_flows(const YAML::Node& config);

    SimulatorVariant m_simulator;
    std::filesystem::path m_topology_config_path;
    std::string m_multipath_type;
};

}  // namespace sim
