#include "simulator.hpp"

namespace sim {

bool Simulator::add_host(std::shared_ptr<IHost> host) {
    if (host == nullptr) {
        return false;
    }
    return m_hosts.insert(host).second;
}

bool Simulator::add_switch(std::shared_ptr<ISwitch> switch_device) {
    if (switch_device == nullptr) {
        return false;
    }
    return m_switches.insert(switch_device).second;
}

bool Simulator::add_connection(std::shared_ptr<IConnection> connection) {
    if (connection == nullptr) {
        return false;
    }
    if (!m_connections.insert(connection).second) {
        return false;
    }
    return true;
}

bool Simulator::add_link(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    if (!m_links.insert(link).second) {
        return false;
    }
    auto a_from = link->get_from();
    auto a_to = link->get_to();
    a_from->add_outlink(link);
    a_to->add_inlink(link);
    return true;
}

void Simulator::set_scenario(Scenario&& scenario) {
    m_scenario = std::move(scenario);
}

std::vector<std::shared_ptr<IDevice>> Simulator::get_devices() const {
    std::vector<std::shared_ptr<IDevice>> devices;

    for (auto host : m_hosts) {
        devices.push_back(dynamic_pointer_cast<IDevice>(host));
    }

    for (auto swtch : m_switches) {
        devices.push_back(dynamic_pointer_cast<IDevice>(swtch));
    }

    return devices;
}

// Calls BFS for each device to build the routing table
void Simulator::recalculate_paths() {
    for (auto src_device : get_devices()) {
        RoutingTable routing_table =
            bfs(dynamic_pointer_cast<IRoutingDevice>(src_device));
        for (auto [dest_device_id, links] : routing_table) {
            for (auto [link, paths_count] : links) {
                src_device->update_routing_table(dest_device_id, link.lock(),
                                                 paths_count);
            }
        }
    }
}

void Simulator::start() {
    recalculate_paths();

    if (m_stop_time.has_value()) {
        Scheduler::get_instance().add<Stop>(m_stop_time.value());
    }

    m_scenario.start();

    while (Scheduler::get_instance().tick()) {
    }
}

std::unordered_set<std::shared_ptr<IConnection>> Simulator::get_connections()
    const {
    return m_connections;
}

}  // namespace sim
