#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "device.hpp"
#include "link.hpp"
#include "scheduler.hpp"

namespace sim {

class Simulator {
public:
    Simulator();
    ~Simulator();
    std::shared_ptr<IRoutingDevice> add_device(std::string a_name,
                                               DeviceType a_type);
    void add_flow(std::shared_ptr<IRoutingDevice> a_from,
                  std::shared_ptr<IRoutingDevice> a_to);
    void add_link(std::shared_ptr<IRoutingDevice> a_from,
                  std::shared_ptr<IRoutingDevice> a_to,
                  std::uint32_t a_speed_mbps, std::uint32_t a_delay);
    // Clear all events in the Scheduler
    void clear();
    // BFS to update the routing table
    void recalculate_paths();
    // Create a Stop event at a_stop_time and start simulation
    void start(std::uint32_t a_stop_time);

private:
    Scheduler& m_scheduler;
    std::unordered_map<std::string, std::shared_ptr<IRoutingDevice>> m_graph;
    std::vector<IFlow> m_flows;
    std::vector<std::shared_ptr<ILink>> m_links;
};

}  // namespace sim
