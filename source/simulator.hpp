#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "device.hpp"

namespace sim {

class Link;
class Scheduler;
class Flow;

class Simulator {
public:
    Simulator();
    ~Simulator();
    void add_device(std::string a_name, DeviceType a_type);
    void add_link(std::string a_from_name, std::string a_to_name,
                  std::uint32_t a_speed_mbps, std::uint32_t a_delay);
    void add_flow(std::string a_from_name, std::string a_to_name);
    void recalculate_paths();  // BFS to update the routing table
    void start(std::uint32_t a_stop_time);
    void clear();

private:
    Scheduler& m_scheduler;
    std::unordered_map<std::string, Device*> m_graph;
    std::vector<Flow> m_flows;
};

}  // namespace sim
