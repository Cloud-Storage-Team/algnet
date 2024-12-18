#pragma once

#include "packet.hpp"
#include "network_element.hpp"

#include <queue>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include "event.hpp"

/**
 * @brief Host abstraction
 * 
 */
class ServerBase: public RoutingNetworkElement {
protected:
    //! Host id (should be given ONLY by Simulator::GenerateNewID)
    std::uint64_t id;
public:
    ServerBase() = delete;
    ServerBase(std::uint64_t id): id(id) {};

    std::uint64_t GetID() const {
        return id;
    }

    /**
     * @brief Generates new packets from host
     * 
     */
    virtual std::uint64_t SendPackets(std::uint64_t current_time_ns, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) = 0;
    
    /**
     * @brief Returns some statistics, calculated by host
     * 
     */
    virtual std::string ToString() const = 0;
    virtual void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) override = 0;
    virtual ~ServerBase() {}
};