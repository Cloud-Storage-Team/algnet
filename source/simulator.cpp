#include "simulator.hpp"

#include <set>

#include "event.hpp"
#include "link.hpp"
#include "receiver.hpp"
#include "scheduler.hpp"
#include "sender.hpp"
#include "switch.hpp"

namespace sim {

Simulator::Simulator() : m_scheduler(Scheduler::get_instance()) {}

Simulator::~Simulator() {
    for (auto& [_, device] : m_graph) {
        std::vector<Device*> neighbors = device->get_neighbors();
        for (Device* neighbor : neighbors) {
            delete device->get_link_to_device(neighbor);
        }
    }
}

Device* Simulator::add_device(std::string a_name, DeviceType a_type) {
    if (m_graph.find(a_name) != m_graph.end()) {
        return nullptr;
    }
    switch (a_type) {
        case DeviceType::SENDER:
            m_graph[a_name] = std::make_unique<Sender>();
            break;
        case DeviceType::SWITCH:
            m_graph[a_name] = std::make_unique<Switch>();
            break;
        case DeviceType::RECEIVER:
            m_graph[a_name] = std::make_unique<Receiver>();
            break;
    }
    return m_graph[a_name].get();
}

void Simulator::add_link(Device* a_from, Device* a_to,
                         std::uint32_t a_speed_mbps, std::uint32_t a_delay) {
    Link* link = new Link(a_from, a_to, a_speed_mbps, a_delay);
    a_from->add_outlink(link);
    a_to->add_inlink(link);
}

// returns map, that gives for each meet device its parent in bfs bypass tree
std::unordered_map<Device*, Device*> bfs(Device* start_device) {
    std::unordered_map<Device*, Device*> parent_table;
    std::queue<Device*> queue;
    std::set<Device*> used;
    queue.push(start_device);

    while (!queue.empty()) {
        Device* device = queue.front();
        queue.pop();
        if (used.find(device) != used.end()) {
            continue;
        }
        used.insert(device);
        std::vector<Device*> neighbors = device->get_neighbors();
        for (Device* neighbor : neighbors) {
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
        std::unordered_map<Device*, Device*> parent_table =
            bfs(src_device.get());
        for (auto& [_, dest_device] : m_graph) {
            Device* next_hop = dest_device.get();
            if (parent_table.find(dest_device.get()) == parent_table.end()) {
                src_device->update_routing_table(dest_device.get(), nullptr);
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
