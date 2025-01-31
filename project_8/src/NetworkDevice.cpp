#include "NetworkDevice.hpp"
#include "Link.hpp"
#include <cstdint>
#include <limits>
#include <stdexcept>

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
// ECMP
std::shared_ptr<Link> NetworkDevice::NextLink(std::uint32_t destination_id) const 
{
    auto it = routing_table_.find(destination_id);
    if (it == routing_table_.end() || it->second.empty())
    {
        throw std::runtime_error("Routing error: No valid link found");
    }
    
    const auto& links = it->second;
    std::uint64_t min_distance = std::numeric_limits<std::uint64_t>::max();
    std::vector<std::shared_ptr<Link>> min_links;
    
    for (const auto& link : links)
    {
        min_distance = std::min(min_distance, link->GetDistance());
    }
    
    std::vector<std::shared_ptr<Link>> min_links;
    for (const auto& link : links)
    {
        if (link->GetDistance() == min_distance)
        {
            min_links.push_back(link);
        }
    }

    if (min_links.empty())
    {
        throw std::runtime_error("Routing error: No valid link found");
    }
    
    std::size_t index = std::hash<std::uint32_t>{}(queue_.front().GetFlowId()) % min_links.size();
    return min_links[index];
}