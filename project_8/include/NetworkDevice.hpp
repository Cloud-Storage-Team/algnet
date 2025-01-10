#pragma once

#include <memory>
#include <cstdint>
#include <queue>
#include "Packet.hpp"
#include "unordered_map"



class NetworkDevice 
{
    public:
        explicit NetworkDevice(std::uint64_t processing_delay_ns);
        void Enqueue(Packet p);
        Packet Dequeue();
        virtual void ProcessPacket(Packet p);
        uint32_t GetId();
        inline static std::uint64_t last_given_device_id = 1;
        uint64_t GetDelay();
        std::shared_ptr<Link> NextLink(std::uint32_t destination_id) const;
        
    protected:
        std::uint32_t id_;
        std::uint64_t completion_time_ = 0;
    private:
        std::unordered_map<std::uint64_t, std::vector<std::shared_ptr<Link>>> routing_table_;
        std::uint64_t processing_delay_ns_;
        std::queue<Packet> queue_;
    
};
