#pragma once

#include "device/interfaces/i_processing_device.hpp"
#include "device/interfaces/i_routing_device.hpp"

namespace sim {

class IReceiver : public virtual IRoutingDevice,
                  public virtual IProcessingDevice {
public:
    virtual ~IReceiver() = default;
};

}  // namespace sim
