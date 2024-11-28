#pragma once

#include <cstdint>
#include <queue>
#include <unordered_map>
#include <vector>

#include "link.hpp"
#include "packet.hpp"
#include "server.hpp"
#include "switch.hpp"

class NetworkSimulator {
  private:
    static std::uint64_t id_to_give;

    // Current time in simulation
    std::uint64_t current_time_ns = 0;
    // Total simulation duration
    std::uint64_t simulation_duration_ns = 1000;

    // Shows next time to call SendPacket method of hosts
    // TODO: store this value for each host in some kind of priority_queue
    std::uint64_t next_ask = 0;

    // Storage of sending servers
    std::unordered_map<std::uint64_t, std::shared_ptr<ServerBase>>
        ids_to_senders{};

    // List of all links in network
    std::vector<std::shared_ptr<Link>> links{};

    // Receiving server
    std::shared_ptr<ServerBase> receiver;

    // Switch in the middle between senders and receiver
    std::shared_ptr<NetworkSwitch> n_switch_1;
    std::shared_ptr<NetworkSwitch> n_switch_2;
    // Sorts packages by sending time
    std::priority_queue<RoutingPacket> packets{};

    /**
     * Creates direct and reverse link between two network elements with
     * given speed (in bits per ns)
     *
     * Do not forget to initialize corresponding routing tables after
     * creating new connection
     */
    Connection
    AddNewConnection(std::shared_ptr<RoutingNetworkElement> source,
                     std::shared_ptr<RoutingNetworkElement> destination,
                     std::uint64_t speed);

    /**
     * Calls SendPackets method of all hosts
     *
     * Called when current simulation time reached next_ask
     */
    void SendPackets(PriorityQueueWrapper &wrapped_packets);

    /**
     * Calls SendPackets method of all hosts
     *
     * Called when current simulation time reached next_ask
     */
    void ProcessNextPacket(PriorityQueueWrapper &wrapped_packets);

  public:
    explicit NetworkSimulator(std::vector<std::shared_ptr<ServerBase>> &senders,
                              std::shared_ptr<ServerBase> receiver,
                              std::shared_ptr<NetworkSwitch> n_switch_1,
                              std::shared_ptr<NetworkSwitch> n_switch_2,
                              std::uint64_t simulation_duration_ns);
    ~NetworkSimulator() = default;

    // Starts the simulation
    void StartSimulation();

    // New ID = last given ID + 1
    static std::uint64_t GenerateNewID() {
        return id_to_give++;
    }
};
