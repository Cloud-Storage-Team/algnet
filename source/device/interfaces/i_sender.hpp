#pragma once

#include "device/interfaces/i_routing_device.hpp"
#include "device/interfaces/i_processing_device.hpp"

namespace sim {

class ISender : public IRoutingDevice, public IProcessingDevice {
public:
    virtual ~ISender() = default;
    virtual void enqueue_packet(Packet packet) = 0;
    virtual Time send_data() = 0;
};

}  // namespace sim
