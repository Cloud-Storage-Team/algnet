#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <string>
#include <utility>

#include "simulator.hpp"

namespace sim {

class YamlParser {
public:
    std::pair<Simulator, TimeNs> build_simulator_from_config(
        const std::filesystem::path& path);

private:
    static std::filesystem::path parse_topology_config_path(
        const YAML::Node& config);
    static TimeNs parse_simulation_time(const YAML::Node& config);

    // node - contains information about set of identifiable objects
    // add_func- function to add new object to simulator
    // parse_func- function to parser single object from config
    // message - error message, which will be printed in case of failing to add
    // new object
    template <typename T>
    void process_identifiables(
        const YAML::Node& node,
        std::function<bool(std::shared_ptr<T>)> add_func,
        std::function<std::shared_ptr<T>(const YAML::Node&, const YAML::Node&)>
            parse_func,
        const std::string& message) {
        static_assert(std::is_base_of_v<Identifiable, T>,
                      "T must be Identifiable");

        for (auto it = node.begin(); it != node.end(); ++it) {
            const YAML::Node key_node = it->first;
            const YAML::Node val_node = it->second;

            std::shared_ptr<T> ptr = parse_func(key_node, val_node);
            if (!IdentifierFactory::get_instance().add_object(ptr)) {
                // TODO: think about moving inside add_object and make in return
                // nothing instead of bool
                throw std::runtime_error(fmt::format(
                    "Can not add object with type {}; object with same id ({}) "
                    "already exists",
                    typeid(T).name(), ptr.get()->get_id()));
            }

            if (!add_func(ptr)) {
                throw std::runtime_error(message +
                                         " ID: " + ptr.get()->get_id());
            }
        }
    }

    void process_hosts(const YAML::Node& hosts_node);
    void process_switches(const YAML::Node& switches_node);

    void process_links(const YAML::Node& links_node,
                       const YAML::Node& link_preset_node);
    void process_flows(const YAML::Node& flows_node);

    Simulator m_simulator;
    std::filesystem::path m_topology_config_path;
};

}  // namespace sim
