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
         std::derived_from<TFlow, IFlow> && std::derived_from<TLink, ILink>
class Simulator {
public:
    Simulator() = default;
    ~Simulator() = default;
    Id add_sender(const std::string& name) {
        if (m_senders.contains(name)) {
            throw std::runtime_error(
                fmt::format("device with name {} already exists.", name));
        }
        m_senders[name] = std::make_shared<TSender>(name);
        return name;
    }

    Id add_receiver(const std::string& name) {
        if (m_receivers.contains(name)) {
            throw std::runtime_error(
                fmt::format("device with name {} already exists.", name));
        }
        m_receivers[name] = std::make_shared<TReceiver>(name);
        return name;
    }

    Id add_switch(const std::string& name) {
        if (m_switches.contains(name)) {
            throw std::runtime_error(
                fmt::format("device with name {} already exists.", name));
        }
        m_switches[name] = std::make_shared<TSwitch>(name);
        return name;
    }

    Id add_flow(const std::string& name, const std::string& sender_name,
                const std::string& receiver_name, Size packet_size,
                Time delay_between_packets, std::uint32_t packets_to_send) {
        auto sender_it = m_senders.find(sender_name);
        if (sender_it == m_senders.end()) {
            throw std::runtime_error(fmt::format(
                "sender with name {} does not exist.", sender_name));
        }
        auto receiver_it = m_receivers.find(receiver_name);
        if (receiver_it == m_receivers.end()) {
            throw std::runtime_error(fmt::format(
                "receiver with name {} does not exist.", receiver_name));
        }
        auto flow = std::make_shared<Flow>(
            name, sender_it->second, receiver_it->second, packet_size,
            delay_between_packets, packets_to_send);
        m_flows.emplace_back(flow);
        return flow->get_id();
    }

    Id add_link(const std::string& name, const std::string& from_name,
                const std::string& to_name, std::uint32_t a_speed_mbps,
                Time a_delay) {
        auto from_device = find_device(from_name);
        auto to_device = find_device(to_name);
        auto link = std::make_shared<TLink>(name, from_device, to_device,
                                            a_speed_mbps, a_delay);
        m_links.emplace_back(link);
        from_device->add_outlink(link);
        to_device->add_inlink(link);
        return link->get_id();
    }

    // Calls BFS for each device to build the routing table
    void recalculate_paths() const {
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

protected:
    std::shared_ptr<TSender> _get_sender(Id id) {
        if (!m_senders.contains(id)) {
            LOG_WARN(fmt::format("device with name {} does not exist.", id));
            return nullptr;
        }
        return m_senders[id];
    }

    std::shared_ptr<TReceiver> _get_receiver(Id id) {
        if (!m_receivers.contains(id)) {
            LOG_WARN(fmt::format("device with name {} does not exist.", id));
            return nullptr;
        }
        return m_receivers[id];
    }

    std::shared_ptr<TSwitch> _get_switch(Id id) {
        if (!m_switches.contains(id)) {
            LOG_WARN(fmt::format("device with name {} does not exist.", id));
            return nullptr;
        }
        return m_switches[id];
    }

    std::shared_ptr<TFlow> _get_flow(Id id) {
        const auto it = std::find_if(
            m_flows.begin(), m_flows.end(),
            [id](const auto& flow) { return flow->get_id() == id; });
        if (it != m_flows.end()) {
            return *it;
        }
        LOG_WARN(fmt::format("flow with id {} does not exist.", id));
        return nullptr;
    }

private:
    std::unordered_map<Id, std::shared_ptr<TSender>> m_senders;
    std::unordered_map<Id, std::shared_ptr<TReceiver>> m_receivers;
    std::unordered_map<Id, std::shared_ptr<TSwitch>> m_switches;
    std::vector<std::shared_ptr<TFlow>> m_flows;
    std::vector<std::shared_ptr<TLink>> m_links;

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

    std::shared_ptr<IRoutingDevice> find_device(Id id) {
        if (m_senders.contains(id)) {
            return m_senders[id];
        }
        if (m_receivers.contains(id)) {
            return m_receivers[id];
        }
        if (m_switches.contains(id)) {
            return m_switches[id];
        }
        throw std::runtime_error(
            fmt::format("device with id {} does not exist.", id));
    }
};

using BasicSimulator = Simulator<Sender, Switch, Receiver, Flow, Link>;

}  // namespace sim
