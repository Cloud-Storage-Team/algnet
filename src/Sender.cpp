#include "Sender.hpp"
#include "NetworkSimulator.hpp"

#include <iostream>

Sender::Sender(double processing_delay_ns):
    NetworkDevice(DeviceType::Sender, processing_delay_ns) {
    id = NetworkDevice::last_given_device_id++;
}

void Sender::ProcessPacket(Packet p) {
    Enqueue(p);
    std::shared_ptr<NetworkDevice> next_device = NextDevice();

    double latency = 0.0;
    /* Queueing delay */
    if (next_processing_time_ns > NetworkSimulator::Now()) {
        latency += next_processing_time_ns - NetworkSimulator::Now();
    }
    /* Processing delay */
    latency += processing_delay_ns;

    if (!p.m_is_ack) {
        double link_last_process_time = NetworkSimulator::GetLinkLastProcessTime(id, next_device->id);
        /* Waiting for link to process previous packets */
        if (link_last_process_time > NetworkSimulator::Now()) {
            latency += link_last_process_time - NetworkSimulator::Now();
        }
        /* Transmission delay */
        latency += NetworkSimulator::GetDistanceNs(id, next_device->id);
        /* Update link last process time */
        NetworkSimulator::UpdateLinkLastProcessTime(id, next_device->id, NetworkSimulator::Now() + latency);
    }

    NetworkSimulator::Schedule(latency, [this, next_device, latency]() {
        Packet p = Dequeue();
        if (p.m_is_ack) {
            std::cout << "[INFO]: ACK received. RTT = " << (NetworkSimulator::Now() - p.m_sending_time_ns) << " ns.\n";
        }
        else {
            /* Packet sending time*/
            p.m_sending_time_ns = NetworkSimulator::Now() - NetworkSimulator::GetDistanceNs(id, next_device->id);
            next_device->ProcessPacket(p);
        }
    });
    next_processing_time_ns = NetworkSimulator::Now() + latency;
}