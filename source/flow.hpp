#pragma once
#include <cstdint>

namespace sim {

class Device;
class Packet;

class Flow {
public:
    // Start at time
    void start(std::uint32_t time);

    // Try to generate a new packet if the internal state allows to do so.
    // by placing it into the flow buffer of the source node.
    // Schedule the next generation event.
    bool try_to_generate(std::uint32_t packet_size);

    // Update the internal state according to some congestion control algorithm
    // Call try_to_generate upon the update
    void update();

private:
    Device *m_src;
    std::uint32_t m_nacks;
    float m_cwnd;
    std::uint32_t m_sent_bytes;
};

}  // namespace sim
