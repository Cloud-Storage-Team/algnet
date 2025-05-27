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
#include "metrics_collector.hpp"
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
    std::shared_ptr<TSender> add_sender(std::string name) {
        if (m_senders.contains(name)) {
            LOG_WARN(fmt::format(
                "add_sender failed: device with name {} already exists.",
                name));
            return nullptr;
        }
        m_senders[name] = std::make_shared<TSender>();
        return m_senders[name];
    }

    std::shared_ptr<TReceiver> add_receiver(std::string name) {
        if (m_receivers.contains(name)) {
            LOG_WARN(fmt::format(
                "add_receiver failed: device with name {} already exists.",
                name));
            return nullptr;
        }
        m_receivers[name] = std::make_shared<TReceiver>();
        return m_receivers[name];
    }

    std::shared_ptr<TSwitch> add_switch(std::string name) {
        if (m_switches.contains(name)) {
            LOG_WARN(fmt::format(
                "add_switch failed: device with name {} already exists.",
                name));
            return nullptr;
        }
        m_switches[name] = std::make_shared<TSwitch>();
        return m_switches[name];
    }

    std::shared_ptr<TFlow> add_flow(std::shared_ptr<ISender> sender,
                                    std::shared_ptr<IReceiver> receiver,
                                    Size packet_size,
                                    Time delay_between_packets,
                                    std::uint32_t packets_to_send) {
        auto flow =
            std::make_shared<Flow>(sender, receiver, packet_size,
                                   delay_between_packets, packets_to_send);
        m_flows.emplace_back(flow);
        return flow;
    }

    void add_link(std::shared_ptr<IRoutingDevice> a_from,
                  std::shared_ptr<IRoutingDevice> a_to,
                  std::uint32_t a_speed_gbps, Time a_delay,
                  Size max_egress_buffer_size = 4096,
                  Size max_ingress_buffer_size = 4096) {
        auto link = std::make_shared<TLink>(a_from, a_to, a_speed_gbps, a_delay,
                                            max_egress_buffer_size,
                                            max_ingress_buffer_size);
        m_links.emplace_back(link);
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
    void start(Time a_stop_time, bool export_metrics = false, bool draw_plots = true) {
        recalculate_paths();
        Scheduler::get_instance().add(Stop(a_stop_time));
        constexpr Time start_time = 0;

        for (auto flow : m_flows) {
            Scheduler::get_instance().add(
                StartFlow(start_time, flow));
        }

        for (auto [name, sender] : m_senders) {
            Scheduler::get_instance().add(
                Process(start_time, sender));
            Scheduler::get_instance().add(
                SendData(start_time, sender));
        }

        for (auto [name, receiver] : m_receivers) {
            Scheduler::get_instance().add(
                Process(start_time, receiver));
        }

        for (auto [name, swtch] : m_switches) {
            Scheduler::get_instance().add(
                Process(start_time, swtch));
        }
        while (Scheduler::get_instance().tick()) {
        }

        if (export_metrics) {
            MetricsCollector::get_instance().export_metrics_to_files();
        }
        if (draw_plots) {
            MetricsCollector::get_instance().draw_metric_plots();
        }
    }

private:
    std::unordered_map<std::string, std::shared_ptr<TSender>> m_senders;
    std::unordered_map<std::string, std::shared_ptr<TReceiver>> m_receivers;
    std::unordered_map<std::string, std::shared_ptr<TSwitch>> m_switches;
    std::vector<std::shared_ptr<IFlow>> m_flows;
    std::vector<std::shared_ptr<ILink>> m_links;
};

using BasicSimulator = Simulator<Sender, Switch, Receiver, Flow, Link>;

using SimulatorVariant = std::variant<BasicSimulator>;

SimulatorVariant create_simulator(std::string_view algorithm);

}  // namespace sim
