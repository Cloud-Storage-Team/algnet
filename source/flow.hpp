#pragma once
#include <cstdint>

#include "receiver.hpp"
#include "sender.hpp"

namespace sim {

class Flow {
public:
    Flow(ISender *a_src, IReceiver *a_dest, float a_start_cwnd);

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
    ISender *m_src;
    IReceiver *m_dest;
    std::uint32_t m_nacks;
    float m_cwnd;
    std::uint32_t m_sent_bytes;
};

}  // namespace sim
