#pragma once

#include <spdlog/fmt/fmt.h>

#include <unordered_set>
#include <variant>

#include "device/host.hpp"
#include "device/switch.hpp"
#include "flow/flow.hpp"
#include "flow/new_flow.hpp"
#include "flow/new_tcp_flow.hpp"
#include "flow/tcp_flow.hpp"
#include "link/link.hpp"
#include "utils/algorithms.hpp"
#include "utils/validation.hpp"

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

    bool add_sender(std::shared_ptr<TSender> sender) {
        if (sender == nullptr) {
            return false;
        }
        return m_senders.insert(sender).second;
    }

    bool add_receiver(std::shared_ptr<TReceiver> receiver) {
        if (receiver == nullptr) {
            return false;
        }
        return m_receivers.insert(receiver).second;
    }

    bool add_switch(std::shared_ptr<TSwitch> switch_device) {
        if (switch_device == nullptr) {
            return false;
        }
        return m_switches.insert(switch_device).second;
    }

    bool add_flow(std::shared_ptr<TFlow> flow) {
        if (flow == nullptr) {
            return false;
        }
        return m_flows.insert(flow).second;
    }

    bool add_link(std::shared_ptr<TLink> link) {
        if (!is_valid_link(link)) {
            return false;
        }
        if (!m_links.insert(link).second) {
            return false;
        }
        auto a_from = link->get_from();
        auto a_to = link->get_to();
        a_from->add_outlink(link);
        a_to->add_inlink(link);
        return true;
    }

    std::vector<std::shared_ptr<IRoutingDevice>> get_devices() const {
        std::vector<std::shared_ptr<IRoutingDevice>> result;
        result.insert(result.end(), m_senders.begin(), m_senders.end());
        result.insert(result.end(), m_receivers.begin(), m_receivers.end());
        result.insert(result.end(), m_switches.begin(), m_switches.end());
        return result;
    }

    // Calls BFS for each device to build the routing table
    void recalculate_paths() {
        for (auto src_device : get_devices()) {
            RoutingTable routing_table = bfs(src_device);
            for (auto [dest_device_id, links] : routing_table) {
                for (auto [link, paths_count] : links) {
                    src_device->update_routing_table(dest_device_id,
                                                     link.lock(), paths_count);
                }
            }
        }
    }
    // Create a Stop event at a_stop_time and start simulation
    void start(Time a_stop_time) {
        recalculate_paths();
        Scheduler::get_instance().add(std::make_unique<Stop>(a_stop_time));
        constexpr Time start_time = 0;

        for (auto flow : m_flows) {
            Scheduler::get_instance().add(
                std::make_unique<StartFlow>(start_time, flow));
        }

        while (Scheduler::get_instance().tick()) {
        }
    }

private:
    std::unordered_set<std::shared_ptr<TSender>> m_senders;
    std::unordered_set<std::shared_ptr<TReceiver>> m_receivers;
    std::unordered_set<std::shared_ptr<TSwitch>> m_switches;
    std::unordered_set<std::shared_ptr<IFlow>> m_flows;
    std::unordered_set<std::shared_ptr<ILink>> m_links;
};

using NewBasicSimulator = Simulator<Host, Switch, Host, NewFlow, Link>;
using NewTcpSimulator = Simulator<Host, Switch, Host, NewTcpFlow, Link>;

using SimulatorVariant = std::variant<NewBasicSimulator, NewTcpSimulator>;

SimulatorVariant create_simulator(std::string_view algorithm);

}  // namespace sim
