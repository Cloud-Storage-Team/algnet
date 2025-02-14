#pragma once

namespace sim {

class Device;
class Packet;

class Flow {
public:
    // Start at time
    void start(int time);

    // Try to generate a new packet if the internal state allows to do so.
    // by placing it into the flow buffer of the source node.
    // Schedule the next generation event.
    bool try_to_generate(int packet_size);

    // Update the internal state according to some congestion control algorithm
    // Call try_to_generate upon the update
    void update();

private:
    Device *m_src;
    int m_nacks;
    int m_cwnd;
    int m_sent_bytes;
};

}  // namespace sim
