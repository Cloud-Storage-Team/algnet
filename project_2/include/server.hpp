#pragma once

#include "packet.hpp"
#include "express_pass.hpp"
#include "utils.hpp"

#include <cstdint>
#include <unordered_map>
#include <vector>

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
     * @details
     * Method is invoked by simulator if:
     * 1) there is no packets in simulation queue 
     * 2) current time of simulation reached next_ask
     * 
     * @param current_time_ns time in system of getting request to send new packets
     * @param packets_wrapped required for pushing packets back to simulation queue
     * @return std::uint64_t next time when simulator should ask for new packets
     */
    virtual std::uint64_t SendPackets(std::uint64_t current_time_ns, PriorityQueueWrapper& packets_wrapped) = 0;
    
    /**
     * @brief Returns some statistics, calculated by host
     * 
     * @return std::string 
     */
    virtual std::string ToString() const = 0;
    virtual void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) override = 0;
    virtual ~ServerBase() {}
};