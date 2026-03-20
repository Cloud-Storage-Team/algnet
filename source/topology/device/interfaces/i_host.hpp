#pragma once
#include "i_device.hpp"

namespace sim {

class IHost : public virtual IDevice {
public:
    virtual ~IHost() = default;

    // Adds given packet to sending queue
    virtual void enqueue_packet(const Packet& packet) = 0;
};

}  // namespace sim
