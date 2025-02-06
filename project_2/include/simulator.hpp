#pragma once

#include "packet.hpp"
#include "event.hpp"
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

    //! Sorts events by sending time
    EventQueue events;
    
    Connection AddNewConnection(std::shared_ptr<NetworkElement> source, std::shared_ptr<NetworkElement> destination, std::uint64_t speed);
    
    void GenerateNewEvents();

public:
    explicit NetworkSimulator(std::vector<std::shared_ptr<ServerBase>>& senders, std::shared_ptr<ServerBase> receiver, std::shared_ptr<NetworkSwitch> n_switch, std::uint64_t simulation_duration_ns, std::uint64_t links_speed);
    ~NetworkSimulator() = default;

    void StartSimulation();

    bool SetCurrentTime(std::uint64_t possible_new_time);

    static std::uint64_t GenerateNewID() {
        return id_to_give++;
    }
};
