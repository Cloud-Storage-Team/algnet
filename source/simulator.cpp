#include "simulator.hpp"

#include <memory>
#include <set>

#include "device.hpp"
#include "event.hpp"
#include "logger.hpp"
#include "receiver.hpp"
#include "sender.hpp"
#include "switch.hpp"

namespace sim {

Simulator::Simulator() : m_scheduler(Scheduler::get_instance()) {}

std::shared_ptr<IRoutingDevice> Simulator::add_device(std::string a_name,
                                                      DeviceType a_type) {
    if (m_graph.find(a_name) != m_graph.end()) {
        LOG_WARN("add_device failed: device with name {} already exists.",
                     a_name);
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

void Simulator::add_flow(std::shared_ptr<IRoutingDevice> a_from,
                         std::shared_ptr<IRoutingDevice> a_to) {
    ISender* sender = dynamic_cast<ISender*>(a_from.get());
    IReceiver* receiver = dynamic_cast<IReceiver*>(a_to.get());
    if (sender == nullptr) {
        LOG_WARN("add_flow failed: sender is nullptr");
        return;
    }
    
    if (receiver== nullptr) {
        LOG_WARN("add_flow failed: receiver is nullptr");
        return;
    }
    m_flows.emplace_back(std::make_shared<Flow>(sender, receiver, 0.f));
}

void Simulator::add_link(std::shared_ptr<IRoutingDevice> a_from,
                         std::shared_ptr<IRoutingDevice> a_to,
                         std::uint32_t a_speed_mbps, std::uint32_t a_delay) {
    m_links.emplace_back(
        std::make_shared<Link>(a_from, a_to, a_speed_mbps, a_delay));
    a_from->add_outlink(m_links.back());
    a_to->add_inlink(m_links.back());
}

// returns map, that for each meet device gives link to its parent in bfs bypass
// tree
std::unordered_map<std::shared_ptr<IRoutingDevice>, std::shared_ptr<ILink>> bfs(
    std::shared_ptr<IRoutingDevice> start_device) {
    std::unordered_map<std::shared_ptr<IRoutingDevice>, std::shared_ptr<ILink>>
        parent_table;
    std::queue<std::shared_ptr<IRoutingDevice>> queue;
    std::set<std::shared_ptr<IRoutingDevice>> used;
    queue.push(start_device);

    while (!queue.empty()) {
        std::shared_ptr<IRoutingDevice> device = queue.front();
        queue.pop();
        if (used.find(device) != used.end()) {
            continue;
        }
        used.insert(device);
        std::set<std::shared_ptr<ILink>> outlinks = device->get_outlinks();
        for (std::shared_ptr<ILink> link : outlinks) {
            if (used.find(link->get_to()) != used.end()) {
                continue;
            }
            if (!parent_table.contains(link->get_to())) {
                parent_table[link->get_to()] = link;
            }
            queue.push(link->get_to());
        }
    }
    return parent_table;
};

void Simulator::recalculate_paths() {
    for (auto& [src_device_name, src_device] : m_graph) {
        std::unordered_map<std::shared_ptr<IRoutingDevice>,
                           std::shared_ptr<ILink>>
            parent_table = bfs(src_device);
        for (auto& [dest_device_name, dest_device] : m_graph) {
            if (parent_table.find(dest_device) == parent_table.end()) {
                src_device->update_routing_table(dest_device, nullptr);
                continue;
            }
            std::shared_ptr<IRoutingDevice> next_hop = dest_device;
            while (parent_table[next_hop]->get_from() != src_device) {
                next_hop = parent_table[next_hop]->get_from();
            }
            bool status = src_device->update_routing_table(
                dest_device, parent_table[next_hop]);
            LOG_INFO("Update routing table {} --> {}: {}", src_device_name,
                         dest_device_name, (status ? "true" : "false"));
        }
    }
}

void Simulator::start(std::uint32_t a_stop_time) {
    recalculate_paths();
    m_scheduler.add(std::make_unique<Stop>(a_stop_time));
    for (std::shared_ptr<IFlow> flow : m_flows) {
        flow->start(0);
    }
    while (m_scheduler.tick()) {
    }
}

}  // namespace sim
