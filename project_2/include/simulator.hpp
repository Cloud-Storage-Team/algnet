#pragma once

#include "packet.hpp"
#include "switch.hpp"
#include "server.hpp"
#include "link.hpp"

#include <vector>
#include <cstdint>
#include <queue>
#include <unordered_map>

class NetworkSimulator {
private:
    static std::uint64_t id_to_give;

    //! Current time in simulation
    std::uint64_t current_time_ns = 0;
    //! Total simulation duration
    std::uint64_t simulation_duration_ns = 1000;

    //! Shows next time to call SendPacket method of hosts
    // TODO: store this value for each host in some kind of priority_queue
    std::uint64_t next_ask = 0;

    //! Storage of sending servers
    std::unordered_map<std::uint64_t, std::shared_ptr<ServerBase>> ids_to_senders{};

    //! List of all links in network
    std::vector<std::shared_ptr<Link>> links{};
    
    //! Receiving server
    std::shared_ptr<ServerBase> receiver;

    //! Switch in the middle between senders and receiver
    std::shared_ptr<NetworkSwitch> n_switch;

    //! Sorts packages by sending time
    std::priority_queue<RoutingPacket> packets{};
    
    /**
     * @brief Creates direct and reverse link between two network elements with given speed (in bits per ns)
     * 
     * @remark Do not forget to initialize corresponding routing tables after creating new connection
     * 
     * @param source first end of created connection
     * @param destination second end of created connection
     * @param speed the speed of pushing packets into the link
     * @return Connection 
     */
    Connection AddNewConnection(std::shared_ptr<RoutingNetworkElement> source, std::shared_ptr<RoutingNetworkElement> destination, std::uint64_t speed);
    
    /**
     * @brief Calls SendPackets method of all hosts
     * 
     * @remark Called when current simulation time reached next_ask or packet queue is empty
     * 
     * @param wrapped_packets 
     */
    void SendPackets(PriorityQueueWrapper& wrapped_packets);

    /**
     * @brief Calls ReceivePacket method of next network element in packet route
     * 
     * @remark Called when current simulation time reached packet sending time
     * 
     * @param wrapped_packets 
     */
    void ProcessNextPacket(PriorityQueueWrapper& wrapped_packets);

public:
    /**
     * @brief Construct a new Network Simulator
     * 
     * @param senders list of hosts that send data packets
     * @param receiver host which wants to receive data from senders
     * @param n_switch switch between receiver and senders (represents bottle neck)
     * @param simulation_duration_ns time of running the simulation in ns
     */
    explicit NetworkSimulator(std::vector<std::shared_ptr<ServerBase>>& senders, std::shared_ptr<ServerBase> receiver, std::shared_ptr<NetworkSwitch> n_switch, std::uint64_t simulation_duration_ns);
    ~NetworkSimulator() = default;

    /**
     * @brief Starts the simulation
     * 
     */
    void StartSimulation();

    /**
     * @brief Sets current_time_ns
     * 
     * @param possible_new_time the time you want to set
     * @return true if current_time is <= simulation_duration
     */
    bool SetCurrentTime(std::uint64_t possible_new_time);

    /**
     * @brief generates New ID = last given ID + 1
     * 
     * @return std::uint64_t 
     */
    static std::uint64_t GenerateNewID() {
        return id_to_give++;
    }
};
