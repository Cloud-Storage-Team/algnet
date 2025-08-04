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

bool Simulator::add_flow(std::shared_ptr<IFlow> flow) {
    if (flow == nullptr) {
        return false;
    }
    return m_flows.insert(flow).second;
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
                src_device->update_routing_table(dest_device_id,
                                                 link.lock(), paths_count);
            }
        }
    }
}
// Create a Stop event at a_stop_time and start simulation
void Simulator::start(TimeNs a_stop_time) {
    recalculate_paths();
    Scheduler::get_instance().add<Stop>(a_stop_time);
    constexpr TimeNs start_time = TimeNs(0);

    for (auto flow : m_flows) {
        Scheduler::get_instance().add<StartFlow>(start_time, flow);
    }

    while (Scheduler::get_instance().tick()) {
    }
}

// returns summary in format [flow : size of delivered data]
std::unordered_set<std::shared_ptr<IFlow>> Simulator::get_flows() const {
    return m_flows;
}

}  // namespace sim
