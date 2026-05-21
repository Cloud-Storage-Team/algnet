#pragma once
#include <memory>

#include "i_device.hpp"

namespace sim {

class IHost : public virtual IDevice {
public:
    virtual ~IHost() = default;

    // Adds given packet to sending queue
    virtual void enqueue_packet(PacketPtr packet) = 0;
};

}  // namespace sim
