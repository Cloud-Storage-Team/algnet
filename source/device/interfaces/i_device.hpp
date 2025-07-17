#pragma once

#include "i_processing_device.hpp"
#include "i_routing_device.hpp"

namespace sim {

class IDevice : public virtual IRoutingDevice, public virtual IProcessingDevice {
public:
    virtual ~IDevice() = default;

    // Returns true if the total number of packets in inlinks change from 0 to 1 
    virtual bool notify_about_arrival(Time arrival_time) = 0;
};

}  // namespace sim
