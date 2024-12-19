#include "Sender.hpp"
#include "NetworkSimulator.hpp"

#include <iostream>

Sender::Sender(double processing_delay_ns):
    NetworkDevice(DeviceType::Sender, processing_delay_ns) {
    id = NetworkDevice::last_given_device_id++;
}

void Sender::ProcessPacket(Packet p) {
    Enqueue(p);

    double packet_processing_delay_ns = processing_delay_ns;
    if (next_processing_time_ns > NetworkSimulator::Now()) {
        packet_processing_delay_ns += next_processing_time_ns - NetworkSimulator::Now();
    }

    NetworkSimulator::Schedule(packet_processing_delay_ns, [this, packet_processing_delay_ns]() {
        Packet p = Dequeue();
        if (p.m_is_ack) {
            std::cout << "[INFO]: ACK received. RTT = " << (NetworkSimulator::Now() + packet_processing_delay_ns - p.m_sending_time_ns) << " ns.\n";
        }
        else {
            std::shared_ptr<NetworkDevice> next_device = NextDevice();
            p.m_sending_time_ns = NetworkSimulator::Now();
            next_device->ProcessPacket(p);
        }
    });
    next_processing_time_ns = NetworkSimulator::Now() + packet_processing_delay_ns;
}