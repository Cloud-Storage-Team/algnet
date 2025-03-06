#pragma once
#include <cstdint>

#include "device.hpp"

namespace sim {

class Receiver : public Device {
public:
    Receiver();
    ~Receiver() = default;

    // TODO: choose a way of passing time to create new events

    // Process a packet by removing it from the ingress buffer
    // Send an ACK to the egress buffer
    // and schedule next receive event after a delay.
    // Upon receiving send an ACK to the sender.
    // Packets are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    std::uint32_t process() final;
};

}  // namespace sim
