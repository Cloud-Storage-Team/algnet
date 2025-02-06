#pragma once

#include "packet.hpp"
#include "network_element.hpp"

#include <queue>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include "event.hpp"

class ServerBase: public NetworkElement {
protected:
    //! Host id (should be given ONLY by Simulator::GenerateNewID)
    std::uint64_t id;
public:
    ServerBase() = delete;
    ServerBase(std::uint64_t id): id(id) {};

    std::uint64_t GetID() const {
        return id;
    }

    virtual std::uint64_t SendPackets(std::uint64_t current_time_ns, EventQueue& all_events) = 0;
    
    virtual std::string ToString() const = 0;
    virtual void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, EventQueue& all_events) override = 0;
    virtual void ProcessPacket(std::uint64_t current_time_ns, EventQueue& all_events) override = 0;
    virtual ~ServerBase() {}
};