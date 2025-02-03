#pragma once

#include <string>
#include <unordered_map>

#include "scheduler.hpp"

namespace sim {

class Simulator {
public:
    Simulator();
    void create_device(std::string a_name);
    void create_link(std::string a_node1, std::string a_node2, size_t a_delay);
    void start();

private:
    Scheduler m_scheduler;
    std::unordered_map<std::string, Device*> m_graph;
    std::vector<Flow> m_flows;
};

}  // namespace sim
