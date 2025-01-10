#include "NetworkDevice.hpp"
#include "Link.hpp"
#include <cstdint>

explicit NetworkDevice::NetworkDevice(std::uint64_t processing_delay_ns)
{
    std::uint64_t processing_delay_ns_=processing_delay_ns;
    id_=last_given_device_id++;
}
void NetworkDevice::Enqueue(Packet p)
{
    queue_.push(p);
}
Packet NetworkDevice::Dequeue()
{
    Packet p = queue_.front();
    queue_.pop();
    return p;
}
uint32_t NetworkDevice::GetId()
{
    return id_;
}

uint64_t NetworkDevice::GetDelay()
{
    return processing_delay_ns_;
}

std::shared_ptr<Link> NetworkDevice::NextLink(std::uint32_t destination_id) const 
{
    
}