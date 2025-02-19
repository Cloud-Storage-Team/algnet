#pragma once

#include <queue>

#include "device.hpp"

namespace sim {

class Packet;

class Sender : public Device {
public:
    Sender();

    // Process an ACK by removing it from the ingress buffer,
    // update the flow state,
    // and schedule next ACK processing event after a delay.
    // Send the next packet: dequeue from the flow buffer
    // and move it to the egress port (link)
    // ACKs are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    void process();

    void enqueue_packet(Packet packet);

private:
    std::queue<Packet> m_flow_buffer;
};

}  // namespace sim
