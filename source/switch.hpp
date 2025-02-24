#pragma once

#include "device.hpp"

namespace sim {

class Switch : public Device {
public:
    Switch(DeviceType a_type);

    // Process a packet by moving it from ingress to egress
    // and schedule next process event after a delay.
    // Packets are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    void process() override;
};

}  // namespace sim
