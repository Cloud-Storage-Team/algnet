#pragma once

#include <yaml-cpp/yaml.h>

#include <string>

#include "simulator.hpp"

namespace sim {

class YamlParser {
public:
    BasicSimulator parseConfig(const std::string& filename);

private:
    static uint32_t parseThroughput(const std::string& throughput_str);
    static uint32_t parseLatency(const std::string& latency_str);

    void processHosts(const YAML::Node& config, BasicSimulator& simulator);
    void processSwitches(const YAML::Node& config, BasicSimulator& simulator);
    void processLinks(const YAML::Node& config,
                      BasicSimulator& simulator) const;
};

}  // namespace sim
