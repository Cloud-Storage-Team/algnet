#include "simulator.hpp"

#include <memory>
#include <set>

#include "device.hpp"
#include "event.hpp"
#include "receiver.hpp"
#include "sender.hpp"
#include "switch.hpp"

namespace sim {

Simulator::Simulator() : m_scheduler(Scheduler::get_instance()) {}

std::shared_ptr<IRoutingDevice> Simulator::add_device(std::string a_name,
                                                      DeviceType a_type) {
    if (m_graph.find(a_name) != m_graph.end()) {
        return nullptr;
    }
    switch (a_type) {
        case DeviceType::SENDER:
            m_graph[a_name] = std::make_shared<Sender>();
            break;
        case DeviceType::SWITCH:
            m_graph[a_name] = std::make_shared<Switch>();
            break;
        case DeviceType::RECEIVER:
            m_graph[a_name] = std::make_shared<Receiver>();
            break;
    }
    return m_graph[a_name];
}

void Simulator::add_flow(ISender* a_from, IReceiver* a_to) {
    m_flows.emplace_back(a_from, a_to, 0.f);
}

void Simulator::add_link(std::shared_ptr<IRoutingDevice> a_from,
                         std::shared_ptr<IRoutingDevice> a_to,
                         std::uint32_t a_speed_mbps, std::uint32_t a_delay) {
    m_links.emplace_back(
        std::make_shared<Link>(a_from, a_to, a_speed_mbps, a_delay));
    a_from->update_routing_table(a_to, m_links.back());
    a_to->add_inlink(m_links.back());
}

// returns map, that gives for each meet device its parent in bfs bypass tree
std::unordered_map<IRoutingDevice*, IRoutingDevice*> bfs(IRoutingDevice* start_device) {
    std::unordered_map<IRoutingDevice*, IRoutingDevice*> parent_table;
    std::queue<IRoutingDevice*> queue;
    std::set<IRoutingDevice*> used;
    queue.push(start_device);

    while (!queue.empty()) {
        IRoutingDevice* device = queue.front();
        queue.pop();
        if (used.find(device) != used.end()) {
            continue;
        }
        used.insert(device);
        std::vector<IRoutingDevice*> neighbors = device->get_neighbors();
        for (IRoutingDevice* neighbor : neighbors) {
            if (used.find(neighbor) != used.end()) {
                continue;
            }
            parent_table[neighbor] = device;
            queue.push(neighbor);
        }
    }
    return parent_table;
};

void Simulator::recalculate_paths() {
    for (auto& [_, src_device] : m_graph) {
        std::unordered_map<IRoutingDevice*, IRoutingDevice*> parent_table =
            bfs(src_device.get());
        for (auto& [_, dest_device] : m_graph) {
            IRoutingDevice* next_hop = dest_device.get();
            if (parent_table.find(dest_device.get()) == parent_table.end()) {
                src_device->update_routing_table(dest_device, nullptr);
                continue;
            }
            while (parent_table[next_hop] != src_device.get()) {
                next_hop = parent_table[next_hop];
            }
            src_device->update_routing_table(
                dest_device, src_device->get_link_to_device(next_hop));
        }
    }
}

void Simulator::start(std::uint32_t a_stop_time) {
    m_scheduler.add(std::move(std::make_unique<Stop>(a_stop_time)));
    while (m_scheduler.tick()) {
    }
}

}  // namespace sim
