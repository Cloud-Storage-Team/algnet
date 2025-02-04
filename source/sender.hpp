#pragma once

#include <queue>

#include "device.hpp"

namespace sim {

class Sender : public Device {
public:
    // Process an ACK by removing it from the ingress buffer,
    // update the flow state,
    // and schedule next ACK processing event after a delay.
    // Send the next packet: dequeue from the flow buffer
    // and move it to the egress port (link)
    // ACKs are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    void process();

private:
    std::queue<Packet> m_flow_buffer;
};

}  // namespace sim
