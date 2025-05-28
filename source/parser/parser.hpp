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
    std::pair<SimulatorVariant, Time> build_simulator_from_config(
        const std::filesystem::path& path);

private:
    static std::filesystem::path parse_topology_config_path(
        const YAML::Node& config);
    static std::string parse_algorithm(const YAML::Node& config);
    static Time parse_simulation_time(const YAML::Node& config);

    void process_devices(const YAML::Node& config);
    void process_links(const YAML::Node& config);
    void process_flows(const YAML::Node& config);

    SimulatorVariant m_simulator;
    std::filesystem::path m_topology_config_path;
};

}  // namespace sim
