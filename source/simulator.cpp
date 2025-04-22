#include "simulator.hpp"

#include <memory>
#include <set>

#include "device/device.hpp"
#include "event.hpp"
#include "logger/logger.hpp"
#include "scheduler.hpp"

namespace sim {

using routing_table_t =
    std::unordered_map<std::shared_ptr<IRoutingDevice>, std::unordered_map<std::shared_ptr<ILink>, int>>;

std::shared_ptr<Sender> Simulator::add_sender(std::string name) {
    if (m_senders.contains(name)) {
        LOG_WARN("add_sender failed: device with name {} already exists.",
                 name);
        return nullptr;
    }
    m_senders[name] = std::make_shared<Sender>();
    return m_senders[name];
}

std::shared_ptr<Receiver> Simulator::add_receiver(std::string name) {
    if (m_receivers.contains(name)) {
        LOG_WARN("add_receiver failed: device with name {} already exists.",
                 name);
        return nullptr;
    }
    m_receivers[name] = std::make_shared<Receiver>();
    return m_receivers[name];
}

std::shared_ptr<Switch> Simulator::add_switch(std::string name) {
    if (m_switches.contains(name)) {
        LOG_WARN("add_switch failed: device with name {} already exists.",
                 name);
        return nullptr;
    }
    m_switches[name] = std::make_shared<Switch>();
    return m_switches[name];
}

std::shared_ptr<Flow> Simulator::add_flow(std::shared_ptr<ISender> sender,
                                          std::shared_ptr<IReceiver> receiver,
                                          Size packet_size,
                                          Time delay_between_packets,
                                          std::uint32_t packets_to_send) {
    auto flow = std::make_shared<Flow>(sender, receiver, packet_size,
                                       delay_between_packets, packets_to_send);
    m_flows.emplace_back(flow);
    return flow;
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
static routing_table_t bfs(std::shared_ptr<IRoutingDevice>& start_device) {
    routing_table_t routing_table;

    std::queue<std::shared_ptr<IRoutingDevice>> queue;
    std::set<std::shared_ptr<IRoutingDevice>> used;
    std::set<std::shared_ptr<IRoutingDevice>> wave_front;
    wave_front.insert(start_device);
    
    while (!wave_front.empty()) {
        for (auto device: wave_front) {
            queue.push(device);
            used.insert(device);
        }
        wave_front.clear();

        while (!queue.empty()) {
            std::shared_ptr<IRoutingDevice> device = queue.front();
            queue.pop();

            std::set<std::shared_ptr<ILink>> outlinks = device->get_outlinks();
            for (std::shared_ptr<ILink> link : outlinks) {
                auto next_hop = link->get_to();
                auto curr_device = link->get_from();
                if (used.contains(next_hop)) {
                    continue;
                }
                wave_front.insert(next_hop);

                if (curr_device == start_device) {
                    routing_table[next_hop][link] = 1;
                } else {
                    for (auto link_and_paths: routing_table[curr_device]) {
                        routing_table[next_hop][link_and_paths.first] += link_and_paths.second;
                    }
                }
            }
        }
    }

    return routing_table;
};

std::vector<std::shared_ptr<IRoutingDevice>> Simulator::get_devices() const {
    std::vector<std::shared_ptr<IRoutingDevice>> result;
    std::transform(m_senders.begin(), m_senders.end(),
                   std::back_inserter(result),
                   [](const auto& pair) { return pair.second; });

    std::transform(m_receivers.begin(), m_receivers.end(),
                   std::back_inserter(result),
                   [](const auto& pair) { return pair.second; });

    std::transform(m_switches.begin(), m_switches.end(),
                   std::back_inserter(result),
                   [](const auto& pair) { return pair.second; });
    return result;
}

void Simulator::recalculate_paths() {
    for (auto src_device : get_devices()) {
        routing_table_t routing_table = bfs(src_device);
        for (auto [dest_device, paths] : routing_table) {
            src_device->update_routing_table(dest_device, paths);
        }
    }
}

void Simulator::start(Time a_stop_time) {
    recalculate_paths();
    Scheduler::get_instance().add(std::make_unique<Stop>(a_stop_time));
    constexpr Time start_time = 0;

    for (std::shared_ptr<IFlow> flow : m_flows) {
        flow->start(start_time);
    }

    for (auto [name, sender] : m_senders) {
        Scheduler::get_instance().add(
            std::make_unique<Process>(start_time, sender));
        Scheduler::get_instance().add(
            std::make_unique<SendData>(start_time, sender));
    }

    for (auto [name, receiver] : m_receivers) {
        Scheduler::get_instance().add(
            std::make_unique<Process>(start_time, receiver));
    }

    for (auto [name, swtch] : m_switches) {
        Scheduler::get_instance().add(
            std::make_unique<Process>(start_time, swtch));
    }
    while (Scheduler::get_instance().tick()) {
    }
}

}  // namespace sim
