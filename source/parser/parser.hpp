#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <string>
#include <utility>

#include "simulator.hpp"
#include "identifiable_parser/identifiable_parser.hpp"

namespace sim {

class YamlParser {
public:
    std::pair<Simulator, TimeNs> build_simulator_from_config(
        const std::filesystem::path& path);

private:
    static std::filesystem::path parse_topology_config_path(
        const YAML::Node& config);
    static std::string parse_algorithm(const YAML::Node& config);
    static TimeNs parse_simulation_time(const YAML::Node& config);

    template <typename T>
    void process_identifiables(const YAML::Node &node, std::function<bool(std::shared_ptr<T>)> add_func, const std::string& message) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            const YAML::Node key_node = it->first;
            const YAML::Node val_node = it->second;

            std::shared_ptr<T> ptr =
                IdentifieableParser<T>::parse_and_registrate(key_node,
                                                                val_node);
            if (!add_func(ptr)) {
                throw std::runtime_error(message + " ID: " +
                                         ptr.get()->get_id());
            }
        }
    }

    void process_hosts(const YAML::Node& hosts_node);
    void process_switches(const YAML::Node& switches_node);

    void process_links(const YAML::Node& links_node);
    void process_flows(const YAML::Node& flows_node);

    Simulator m_simulator;
    std::filesystem::path m_topology_config_path;
};

}  // namespace sim
