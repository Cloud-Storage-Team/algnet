#include "Switch.hpp"
#include "NetworkSimulator.hpp"

Switch::Switch(double processing_delay_ns):
    NetworkDevice(DeviceType::Switch, processing_delay_ns) {
    id = NetworkDevice::last_given_device_id++;
}

void Switch::ProcessPacket(Packet p) {
    Enqueue(p);
    std::shared_ptr<NetworkDevice> next_device = NextDevice();

    double packet_processing_delay_ns = processing_delay_ns;
    if (next_processing_time_ns > NetworkSimulator::Now()) {
        packet_processing_delay_ns += next_processing_time_ns - NetworkSimulator::Now();
    }

    NetworkSimulator::Schedule(packet_processing_delay_ns, [this, next_device]() {
        Packet p = Dequeue();
        next_device->ProcessPacket(p);
    });
    next_processing_time_ns = NetworkSimulator::Now() + packet_processing_delay_ns;
}