#pragma once

#include "packet.hpp"
#include "express_pass.hpp"
#include "utils.hpp"

#include <cstdint>
#include <unordered_map>
#include <vector>

// Host abstraction
class ServerBase: public RoutingNetworkElement {
protected:
    // Host id (should be given ONLY by Simulator::GenerateNewID)
    std::uint64_t id;
public:
    std::uint64_t GetID() {
        return id;
    }

    /**
     * Generates new packets from host
     * 
     * Method is invoked by simulator if:
     * 1) there is no packets in simulation queue 
     * 2) current time of simulation is equal to next_ask
     *
     * @param current_time_ns current time in simulation
     * @param packets_wrapped required for returning updated packets back to simulation queue
     * @return next time when simulator should ask for new packets
     */
    virtual std::uint64_t SendPackets(std::uint64_t current_time_ns, PriorityQueueWrapper& packets_wrapped) = 0;
    virtual void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) override = 0;
    virtual ~ServerBase() {}
};