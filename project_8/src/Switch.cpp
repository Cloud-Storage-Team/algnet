#pragma once
#include "Switch.hpp"
#include "Packet.hpp"
#include "NetworkSimulator.hpp"
Switch::Switch(std::uint64_t processing_delay_ns):
    NetworkDevice(processing_delay_ns) 
    {
        id_ = NetworkDevice::last_given_device_id++;
    }

void Switch::ProcessPacket(Packet p) 
{
    Enqueue(p);
    std::shared_ptr<Link> link;

    if (p.IsAck()) 
    {
        std::uint64_t id =p.GetSourceID();
        std::vector<std::shared_ptr<Link>> links;
        
    }
    else 
    {
        /* send packet to receiver */
        //link = NextLink(p.GEt);
    }
    //std::shared_ptr<NetworkDevice> next_device = link->destination;

    /* Latency is a sum of processing and queuing delays */
    std::uint64_t latency = GetDelay() + std::max<std::uint64_t>(0, completion_time_ - NetworkSimulator::NetworkSimulator::Now());
    /* Waiting for the link to process previous packets, then add distance (i.e. link transmission time) */
    std::uint64_t transmission_delay = (std::uint64_t)std::max<std::uint64_t>(0, (std::int64_t)(link->GetLastProcessTime() - (NetworkSimulator::NetworkSimulator::Now() + latency))) +
                                       link->GetDistance();
    /* Update link last process time */
    link->UpdateLastProcessTime(NetworkSimulator::NetworkSimulator::Now() + latency + transmission_delay);

    NetworkSimulator::NetworkSimulator::Schedule(latency + transmission_delay, [this, next_device]() 
    {
        Packet p = Dequeue();
        next_device->ProcessPacket(p);
    });
    completion_time_ = NetworkSimulator::NetworkSimulator::Now() + latency;
}