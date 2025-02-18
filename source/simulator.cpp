#include "simulator.hpp"

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
    for (auto& [_, device] : m_graph) {
        delete device;
    }
}

void Simulator::add_device(std::string a_name, DeviceType a_type) {
    if (m_graph.find(a_name) != m_graph.end()) {
        return;
    }
    switch (a_type) {
        case DeviceType::SENDER:
            m_graph[a_name] = new Sender();
            break;
        case DeviceType::SWITCH:
            m_graph[a_name] = new Switch();
            break;
        case DeviceType::RECEIVER:
            m_graph[a_name] = new Receiver();
            break;
    }
}

void Simulator::add_link(std::string a_from_name, std::string a_dest_name,
                         std::uint32_t a_speed_mbps, std::uint32_t a_delay) {
    Link* link = new Link(m_graph[a_from_name], m_graph[a_dest_name],
                          a_speed_mbps, a_delay);
    m_graph[a_from_name]->add_outlink(link);
    m_graph[a_dest_name]->add_inlink(link);
}

void Simulator::recalculate_paths() {
    auto bfs = [](Device* a_vertex) {
        std::unordered_map<Device*, Device*> path;
        std::queue<Device*> q;
        q.push(a_vertex);

        while (!q.empty()) {
            Device* vertex = q.front();
            q.pop();
            std::vector<Device*> neighbors = vertex->get_neighbors();
            for (Device* neighbor : neighbors) {
                path[neighbor] = vertex;
                q.push(neighbor);
            }
        }
        return path;
    };

    for (auto& [_, src_device] : m_graph) {
        std::unordered_map<Device*, Device*> path = bfs(src_device);
        for (auto& [_, dest_device] : m_graph) {
            Device* next_hop = dest_device;
            while (path[next_hop] != src_device) {
                next_hop = path[next_hop];
            }
            src_device->update_routing_table(
                dest_device, src_device->get_link_to_device(next_hop));
        }
    }
}

void Simulator::start(std::uint32_t a_stop_time) {
    while (m_scheduler.pick_time() <= a_stop_time) {
        m_scheduler.tick();
    }
}

void Simulator::clear() { m_scheduler.clear(); }

}  // namespace sim
