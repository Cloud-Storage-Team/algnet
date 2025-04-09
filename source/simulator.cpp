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

using routing_table_t =
    std::unordered_map<std::shared_ptr<IRoutingDevice>, std::shared_ptr<ILink>>;

Simulator::Simulator() : m_scheduler(Scheduler::get_instance()) {}

std::shared_ptr<IRoutingDevice> Simulator::add_device(std::string a_name,
                                                      DeviceType a_type) {
    if (m_graph.contains(a_name)) {
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
                         std::shared_ptr<IRoutingDevice> a_to,
                         Time delay_between_packets, Size packet_size) {
    ISender* sender = dynamic_cast<ISender*>(a_from.get());
    IReceiver* receiver = dynamic_cast<IReceiver*>(a_to.get());
    if (sender == nullptr) {
        LOG_WARN("add_flow failed: sender is nullptr");
        return;
    }

    if (receiver == nullptr) {
        LOG_WARN("add_flow failed: receiver is nullptr");
        return;
    }
    m_flows.emplace_back(std::make_shared<Flow>(sender, receiver, packet_size,
                                                delay_between_packets));
}

void Simulator::add_link(std::shared_ptr<IRoutingDevice> a_from,
                         std::shared_ptr<IRoutingDevice> a_to,
                         std::uint32_t a_speed_mbps, Time a_delay) {
    auto link = std::make_shared<Link>(a_from, a_to, a_speed_mbps, a_delay);
    m_links.emplace_back(link);
    a_from->add_outlink(link);
    a_to->add_inlink(link);
}

// returns start device routing table
routing_table_t bfs(std::shared_ptr<IRoutingDevice>& start_device) {
    routing_table_t routing_table;
    std::queue<std::shared_ptr<IRoutingDevice>> queue;
    std::set<std::shared_ptr<IRoutingDevice>> used;
    queue.push(start_device);

    while (!queue.empty()) {
        std::shared_ptr<IRoutingDevice> device = queue.front();
        queue.pop();
        if (used.contains(device)) {
            continue;
        }
        used.insert(device);
        std::set<std::shared_ptr<ILink>> outlinks = device->get_outlinks();
        for (std::shared_ptr<ILink> link : outlinks) {
            auto next_hop = link->get_to();
            auto curr_device = link->get_from();
            if (used.contains(next_hop)) {
                continue;
            }
            if (curr_device == start_device) {
                routing_table[next_hop] = link;
            } else if (!routing_table.contains(next_hop)) {
                routing_table[next_hop] = routing_table[curr_device];
            }
            queue.push(next_hop);
        }
    }
    return routing_table;
};

void Simulator::recalculate_paths() {
    for (auto& [_, src_device] : m_graph) {
        routing_table_t routing_table = bfs(src_device);
        for (auto [dest_device, link] : routing_table) {
            src_device->update_routing_table(dest_device, link);
        }
    }
}

void Simulator::start(Time a_stop_time) {
    recalculate_paths();
    m_scheduler.add(std::make_unique<Stop>(a_stop_time));
    for (std::shared_ptr<IFlow> flow : m_flows) {
        flow->start(0);
    }
    while (m_scheduler.tick()) {
    }
}

}  // namespace sim
