#include "NetworkDevice.hpp"

NetworkDevice::NetworkDevice(DeviceType type, double processing_delay_ns):
    type(type),
    processing_delay_ns(processing_delay_ns) { }

void NetworkDevice::Enqueue(Packet p) {
    buffer.push(p);
}

Packet NetworkDevice::Dequeue() {
    Packet result = buffer.front();
    buffer.pop();
    return result;
}

bool NetworkDevice::Empty() {
    return buffer.empty();
}