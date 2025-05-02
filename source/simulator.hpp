#pragma once

#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <concepts>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "device/device.hpp"
#include "device/receiver.hpp"
#include "device/sender.hpp"
#include "device/switch.hpp"
#include "link.hpp"
#include "logger/logger.hpp"
#include "scheduler.hpp"
#include "utils/algorithms.hpp"

namespace sim {

template <typename TSender, typename TSwitch, typename TReceiver,
          typename TFlow, typename TLink>
requires std::derived_from<TSender, ISender> &&
         std::derived_from<TSwitch, ISwitch> &&
         std::derived_from<TReceiver, IReceiver> &&
         std::derived_from<TFlow, IFlow> &&
         std::derived_from<TLink, ILink>
class Simulator {
public:
    Simulator() = default;
    ~Simulator() = default;
    Id add_sender(std::string name) {
        if (m_name_to_id.contains(name)) {
            throw std::runtime_error(
                fmt::format("device with name {} already exists.", name));
        }
        auto sender = std::make_shared<TSender>();
        Id sender_id = sender->get_id();
        
        m_senders[sender_id] = sender;
        m_id_to_name[sender_id] = name;
        m_name_to_id[name] = sender_id;
        return sender_id;
    }

    Id add_receiver(std::string name) {
        if (m_name_to_id.contains(name)) {
            throw std::runtime_error(
                fmt::format("device with name {} already exists.", name));
        }
        auto receiver = std::make_shared<TReceiver>();
        Id receiver_id = receiver->get_id();
        
        m_receivers[receiver_id] = receiver;
        m_id_to_name[receiver_id] = name;
        m_name_to_id[name] = receiver_id;
        return receiver_id;
    }

    Id add_switch(std::string name) {
        if (m_name_to_id.contains(name)) {
            throw std::runtime_error(fmt::format(
                "device with name {} already exists.",
                name));
        }
        auto swtch = std::make_shared<TSwitch>();
        Id switch_id = swtch->get_id();
        
        m_switches[switch_id] = swtch;
        m_id_to_name[switch_id] = name;
        m_name_to_id[name] = switch_id;
        return switch_id;
    }

    Id add_flow(std::string sender_name, std::string receiver_name,
                Size packet_size, Time delay_between_packets,
                std::uint32_t packets_to_send) {
        auto sender_it = m_senders.find(m_name_to_id[sender_name]);
        if (sender_it == m_senders.end()) {
            throw std::runtime_error(fmt::format(
                "sender with name {} does not exist.", sender_name));
        }
        auto receiver_it = m_receivers.find(m_name_to_id[receiver_name]);
        if (receiver_it == m_receivers.end()) {
            throw std::runtime_error(fmt::format(
                "receiver with name {} does not exist.", receiver_name));
        }
        auto flow =
            std::make_shared<Flow>(sender_it->second, receiver_it->second, packet_size,
                                   delay_between_packets, packets_to_send);
        m_flows.emplace_back(flow);
        return flow->get_id();
    }

    Id add_link(std::shared_ptr<IRoutingDevice> a_from,
                  std::shared_ptr<IRoutingDevice> a_to,
                  std::uint32_t a_speed_mbps, Time a_delay) {
        auto link =
            std::make_shared<TLink>(a_from, a_to, a_speed_mbps, a_delay);
        m_links.emplace_back(link);
        a_from->add_outlink(link);
        a_to->add_inlink(link);
        return link->get_id();
    }

    std::shared_ptr<TSender> get_sender(Id id) {
        if (!m_senders.contains(id)) {
            LOG_WARN(fmt::format("device with name {} does not exist.", m_id_to_name[id]));
            return nullptr;
        }
        return m_senders[id];
    }

    std::shared_ptr<TReceiver> get_receiver(Id id) {
        if (!m_receivers.contains(id)) {
            LOG_WARN(fmt::format("device with name {} does not exist.", m_id_to_name[id]));
            return nullptr;
        }
        return m_receivers[id];
    }

    std::shared_ptr<TSwitch> get_switch(Id id) {
        if (!m_switches.contains(id)) {
            LOG_WARN(fmt::format("device with name {} does not exist.", m_id_to_name[id]));
            return nullptr;
        }
        return m_switches[id];
    }

    std::shared_ptr<TFlow> get_flow(Id id) {
        const auto it = std::find_if(
            m_flows.begin(), m_flows.end(),
            [id](const auto& flow) { return flow->get_id() == id; });
        if (it != m_flows.end()) {
            return *it;
        }
        LOG_WARN(fmt::format("flow with id {} does not exist.", id));
        return nullptr;
    }

    // Calls BFS for each device to build the routing table
    void recalculate_paths() {
        for (auto src_device : get_devices()) {
            RoutingTable routing_table = bfs(src_device);
            for (auto [dest_device, link] : routing_table) {
                src_device->update_routing_table(dest_device, link);
            }
        }
    }
    // Create a Stop event at a_stop_time and start simulation
    void start(Time a_stop_time) {
        recalculate_paths();
        Scheduler::get_instance().add(std::make_unique<Stop>(a_stop_time));
        constexpr Time start_time = 0;

        for (auto flow : m_flows) {
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

private:
    std::unordered_map<Id, std::shared_ptr<TSender>> m_senders;
    std::unordered_map<Id, std::shared_ptr<TReceiver>> m_receivers;
    std::unordered_map<Id, std::shared_ptr<TSwitch>> m_switches;
    std::vector<std::shared_ptr<TFlow>> m_flows;
    std::vector<std::shared_ptr<TLink>> m_links;

    std::unordered_map<Id, std::string> m_id_to_name;
    std::unordered_map<std::string, Id> m_name_to_id;

    std::vector<std::shared_ptr<IRoutingDevice>> get_devices() const {
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
};

using BasicSimulator = Simulator<Sender, Switch, Receiver, Flow, Link>;

}  // namespace sim
