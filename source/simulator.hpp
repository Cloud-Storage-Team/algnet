#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "device.hpp"
#include "scheduler.hpp"

namespace sim {

class Simulator {
public:
    Simulator();
    ~Simulator();
    Device* add_device(std::string a_name, DeviceType a_type);
    void add_flow(Device* a_from, Device* a_to);
    void add_link(Device* a_from, Device* a_to, std::uint32_t a_speed_mbps,
                  std::uint32_t a_delay);
    // Clear all events in the Scheduler
    void clear();
    // BFS to update the routing table
    void recalculate_paths();
    // Create a Stop event at a_stop_time and start simulation
    void start(std::uint32_t a_stop_time);

private:
    Scheduler& m_scheduler;
    std::unordered_map<std::string, Device*> m_graph;
    std::vector<Flow> m_flows;
};

}  // namespace sim
