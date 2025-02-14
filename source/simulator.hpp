#pragma once

#include <string>
#include <unordered_map>

#include "scheduler.hpp"

namespace sim {

class Simulator {
public:
    Simulator();
    void add_device(std::string a_name, DeviceType a_type);
    void add_link(Device* a_node1, Device* a_node2, int a_delay);
    void recalculate_paths();  // BFS to update the routing table
    void start(int a_stop_time);

private:
    Scheduler& m_scheduler;
    std::unordered_map<std::string, Device*> m_graph;
    std::vector<Flow> m_flows;
};

}  // namespace sim
