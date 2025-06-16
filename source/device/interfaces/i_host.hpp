#pragma once
#include "i_routing_device.hpp"
#include "i_processing_device.hpp"

namespace sim {

class IHost : public IRoutingDevice, public IProcessingDevice {
public:
    virtual ~IHost() = default;
    virtual void enqueue_packet(Packet packet) = 0;
    virtual Time send_data() = 0;
};

}  // namespace sim
