#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "device.hpp"
#include "link.hpp"
#include "switch.hpp"

namespace sim {

class Simulator {
public:
    Simulator() = default;
    ~Simulator() = default;
    std::shared_ptr<Sender> add_sender(std::string name);
    std::shared_ptr<Receiver> add_receiver(std::string name);
    std::shared_ptr<Switch> add_switch(std::string name);

    std::shared_ptr<Flow> add_flow(std::shared_ptr<ISender> sender,
                                   std::shared_ptr<IReceiver> recever,
                                   Size a_packet_size,
                                   Time a_delay_between_packets,
                                   std::uint32_t packets_to_send);

    void add_link(std::shared_ptr<IRoutingDevice> a_from,
                  std::shared_ptr<IRoutingDevice> a_to,
                  std::uint32_t a_speed_mbps, Time a_delay);

    std::vector<std::shared_ptr<IRoutingDevice>> get_devices() const;

    // Clear all events in the Scheduler
    void clear();
    // BFS to update the routing table
    void recalculate_paths();
    // Create a Stop event at a_stop_time and start simulation
    void start(Time a_stop_time);

private:
    std::unordered_map<std::string, std::shared_ptr<Sender>> m_senders;
    std::unordered_map<std::string, std::shared_ptr<Receiver>> m_receivers;
    std::unordered_map<std::string, std::shared_ptr<Switch>> m_switches;
    std::vector<std::shared_ptr<IFlow>> m_flows;
    std::vector<std::shared_ptr<ILink>> m_links;
};

}  // namespace sim
