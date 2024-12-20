#pragma once

#include <memory>
#include <cstdint>
#include <queue>
#include "Packet.hpp"
    


enum class DeviceType: std::uint8_t 
{
    Switch,
    Server,
};

class NetworkDevice 
{
    public:
        void Enqueue(Packet p);
        Packet Dequeue();
        virtual void ProcessPacket(Packet p);

    private:
    uint32_t id_;
    std::queue<Packet> queue_;
    
};
