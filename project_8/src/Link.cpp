#include "Link.hpp"
#include <memory>

Link::Link(std::shared_ptr<NetworkDevice> first_device, std::shared_ptr<NetworkDevice> second_device, std::uint64_t distance)
 : first_device_(std::move(first_device)) , second_device_(std::move(second_device)) , distance_(distance){}

void Link::UpdateLastProcessTime(std::uint64_t new_process_time_ns)
{
    last_process_time_ns_ = std::max(last_process_time_ns_, new_process_time_ns);
}

std::uint64_t Link::GetDistance()
{
    return distance_;
}

std::uint64_t Link::GetLastProcessTime()
{
    return last_process_time_ns_;
}

std::shared_ptr<NetworkDevice> Link::GetNextDevice(uint32_t id)
{
    return id == first_device_->GetId() ? second_device_: first_device_;
}

