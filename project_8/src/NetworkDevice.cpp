#include "NetworkDevice.hpp"

void NetworkDevice::Enqueue(Packet p)
{
    queue_.push(p);
}
Packet NetworkDevice::Dequeue()
{

    Packet p =  queue_.front();
    queue_.pop();
    return p;
}
