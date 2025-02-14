#include "simulator.hpp"

#include "link.hpp"
#include "receiver.hpp"
#include "scheduler.hpp"
#include "sender.hpp"
#include "switch.hpp"

namespace sim {

Simulator::Simulator() : m_scheduler(Scheduler::get_instance()) {}

Simulator::~Simulator() {
    for (auto& p : m_graph) {
        delete p.second;
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

void Simulator::add_link(Device* a_node1, Device* a_node2, int a_delay) {
    Link* link = new Link(a_node1, a_delay);
    a_node2->add_inlink(link);
    link = new Link(a_node2, a_delay);
    a_node1->add_inlink(link);
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

void Simulator::start(int a_stop_time) {
    while (m_scheduler.peek_time() <= a_stop_time) {
        m_scheduler.tick();
    }
}

}  // namespace sim
