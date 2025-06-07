#pragma once

#include "device/interfaces/i_routing_device.hpp"
#include "device/interfaces/i_processing_device.hpp"

namespace sim {

class ISwitch : public IRoutingDevice,
                public IProcessingDevice {
public:
    virtual ~ISwitch() = default;
};


}  // namespace sim
