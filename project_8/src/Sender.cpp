#include "Sender.hpp"
#include "NetworkSimulator.hpp"

#include <iostream>

Sender::Sender(std::uint64_t processing_delay_ns):
    NetworkDevice(processing_delay_ns) 
    {
        id_ = NetworkDevice::last_given_device_id++;
    }

void Sender::ProcessPacket(Packet p) 
{
    Enqueue(p);
    /* Next device in the flow path */
    std::shared_ptr<Link> link = NextLink(p.GetDestinationID());
    std::shared_ptr<NetworkDevice> next_device = link->GetNextDevice(id_);
    /* Latency is a sum of processing and queuing delays */
    std::uint64_t latency = GetDelay()+ std::max<std::uint64_t>(0, completion_time_ - NetworkSimulator::NetworkSimulator::Now());

    std::uint64_t transmission_delay = 0;
    /* If ACK flag set to false, send the packet to the next device */
    if (!p.IsAck()) 
    {
        /* Waiting for the link to process previous packets, then add distance (i.e. link transmission time) */
        transmission_delay = (std::uint64_t)std::max<std::int64_t>(0, (std::int64_t)(link->GetLastProcessTime()- (NetworkSimulator::NetworkSimulator::Now() + latency))) +
                             link->GetDistance();
        /* Update link last process time */
        link->UpdateLastProcessTime(NetworkSimulator::NetworkSimulator::Now() + latency + transmission_delay);
    }
    std::shared_ptr<NetworkDevice> next_device = link->destination;

    NetworkSimulator::NetworkSimulator::Schedule(latency + transmission_delay, [this, next_device, transmission_delay]() {
        Packet p = Dequeue();
        if (p.IsAck()) 
        {

        }
        else 
        {
            /* Packet sending time*/
            p.m_sending_time_ns = NetworkSimulator::NetworkSimulator::Now() - transmission_delay;
            next_device->ProcessPacket(p);
        }
    });
    completion_time_ = NetworkSimulator::NetworkSimulator::Now() + latency;
}