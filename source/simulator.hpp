#pragma once

#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <concepts>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "device/device.hpp"
#include "device/receiver.hpp"
#include "device/sender.hpp"
#include "device/switch.hpp"
#include "link.hpp"
#include "logger/logger.hpp"
#include "scheduler.hpp"
#include "utils/algorithms.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

template <typename TSender, typename TSwitch, typename TReceiver,
          typename TFlow, typename TLink>
requires std::derived_from<TSender, ISender> &&
    std::derived_from<TSwitch, ISwitch> &&
    std::derived_from<TReceiver, IReceiver> &&
    std::derived_from<TFlow, IFlow> && std::derived_from<TLink, ILink>
class Simulator {
public:
    using Sender_T = TSender;
    using Switch_T = TSwitch;
    using Receiver_T = TReceiver;
    using Flow_T = TFlow;
    using Link_T = TLink;
    Simulator() = default;
    ~Simulator() = default;

    std::shared_ptr<TSender> add_sender(Id sender_id) {
        if (m_senders.contains(sender_id)) {
            LOG_WARN(fmt::format(
                "add_sender failed: device with name {} already exists.",
                sender_id));
            return nullptr;
        }
        std::shared_ptr<TSender> sender =
            IdentifierFactory::get_instance().get_object<TSender>(sender_id);
        if (sender == nullptr) {
            return nullptr;
        }
        m_senders[sender_id] = sender;
        return m_senders[sender_id];
    }

    std::shared_ptr<TReceiver> add_receiver(Id receiver_id) {
        if (m_receivers.contains(receiver_id)) {
            LOG_WARN(fmt::format(
                "add_receiver failed: device with name {} already exists.",
                receiver_id));
            return nullptr;
        }
        std::shared_ptr<TReceiver> receiver =
            IdentifierFactory::get_instance().get_object<TReceiver>(
                receiver_id);
        if (receiver == nullptr) {
            return nullptr;
        }
        m_receivers[receiver_id] = receiver;
        return m_receivers[receiver_id];
    }

    std::shared_ptr<TSwitch> add_switch(Id switch_id) {
        if (m_switches.contains(switch_id)) {
            LOG_WARN(fmt::format(
                "add_switch failed: device with name {} already exists.",
                switch_id));
            return nullptr;
        }
        std::shared_ptr<TSwitch> switch_device =
            IdentifierFactory::get_instance().get_object<TSwitch>(switch_id);
        if (switch_device == nullptr) {
            return nullptr;
        }
        m_switches[switch_id] = switch_device;
        return m_switches[switch_id];
    }

    std::shared_ptr<TFlow> add_flow(Id flow_id) {
        std::shared_ptr<TFlow> flow =
            IdentifierFactory::get_instance().get_object<TFlow>(flow_id);
        if (flow == nullptr) {
            return nullptr;
        }
        m_flows.push_back(flow);
        return flow;
    }

    void add_link(Id link_id) {
        std::shared_ptr<TLink> link =
            IdentifierFactory::get_instance().get_object<TLink>(link_id);
        if (link == nullptr) {
            LOG_ERROR(fmt::format(
                "Could not fild link with id {} in Identifier factory",
                link_id));
        }
        m_links.emplace_back(link);
        auto a_from = link->get_from();
        auto a_to = link->get_to();
        a_from->add_outlink(link);
        a_to->add_inlink(link);
    }

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

    // Calls BFS for each device to build the routing table
    void recalculate_paths() {
        for (auto src_device : get_devices()) {
            RoutingTable routing_table = bfs(src_device);
            for (auto [dest_device, links] : routing_table) {
                for (auto [link, paths_count] : links) {
                    src_device->update_routing_table(dest_device, link,
                                                     paths_count);
                }
            }
        }
    }
    // Create a Stop event at a_stop_time and start simulation
    void start(Time a_stop_time) {
        recalculate_paths();
        Scheduler::get_instance().add(Stop(a_stop_time));
        constexpr Time start_time = 0;

        for (auto flow : m_flows) {
            Scheduler::get_instance().add(StartFlow(start_time, flow));
        }

        for (auto [name, sender] : m_senders) {
            Scheduler::get_instance().add(Process(start_time, sender));
            Scheduler::get_instance().add(SendData(start_time, sender));
        }

        for (auto [name, receiver] : m_receivers) {
            Scheduler::get_instance().add(Process(start_time, receiver));
        }

        for (auto [name, swtch] : m_switches) {
            Scheduler::get_instance().add(Process(start_time, swtch));
        }
        while (Scheduler::get_instance().tick()) {
        }
    }

private:
    std::unordered_map<Id, std::shared_ptr<TSender>> m_senders;
    std::unordered_map<Id, std::shared_ptr<TReceiver>> m_receivers;
    std::unordered_map<Id, std::shared_ptr<TSwitch>> m_switches;
    std::vector<std::shared_ptr<IFlow>> m_flows;
    std::vector<std::shared_ptr<ILink>> m_links;
};

using BasicSimulator = Simulator<Sender, Switch, Receiver, Flow, Link>;

using SimulatorVariant = std::variant<BasicSimulator>;

SimulatorVariant create_simulator(std::string_view algorithm);

}  // namespace sim
