#pragma once

#include <spdlog/fmt/fmt.h>

#include <map>
#include <unordered_set>
#include <variant>

#include "device/host.hpp"
#include "device/switch.hpp"
#include "event/start_connection.hpp"
#include "event/stop.hpp"
#include "connection/i_connection.hpp"
#include "flow/tcp/tcp_flow.hpp"
#include "link/link.hpp"
#include "utils/algorithms.hpp"
#include "utils/validation.hpp"

namespace sim {

class Simulator {
public:
    Simulator() = default;
    ~Simulator() = default;

    bool add_host(std::shared_ptr<IHost> host);

    bool add_switch(std::shared_ptr<ISwitch> switch_device);

    bool add_connection(std::shared_ptr<IConnection> connection);

    bool add_link(std::shared_ptr<ILink> link);

    std::vector<std::shared_ptr<IDevice>> get_devices() const;

    // Calls BFS for each device to build the routing table
    void recalculate_paths();
    // Create a Stop event at a_stop_time and start simulation
    void start(TimeNs a_stop_time);

    std::unordered_set<std::shared_ptr<IConnection>> get_connections() const;

private:
    std::unordered_set<std::shared_ptr<IHost>> m_hosts;
    std::unordered_set<std::shared_ptr<ISwitch>> m_switches;
    std::unordered_set<std::shared_ptr<IConnection>> m_connections;
    std::unordered_set<std::shared_ptr<ILink>> m_links;
};

}  // namespace sim
