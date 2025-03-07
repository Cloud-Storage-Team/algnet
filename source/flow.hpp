#pragma once
#include <cstdint>

#include "receiver.hpp"
#include "sender.hpp"

namespace sim {

class Flow {
public:
    Flow() = default;
    virtual ~Flow() = default;

    // Start at time
    virtual void start(std::uint32_t time) = 0;

    // Try to generate a new packet if the internal state allows to do so.
    // by placing it into the flow buffer of the source node.
    // Schedule the next generation event.
    virtual bool try_to_generate(std::uint32_t packet_size) = 0;

    // Update the internal state according to some congestion control algorithm
    // Call try_to_generate upon the update
    virtual void update() = 0;

    virtual IReceiver *get_destination() const = 0;

    // TODO: move to realisation
    //  private:
    //      ISender *m_src;
    //      IReceiver *m_dest;
    //      std::uint32_t m_nacks;
    //      float m_cwnd;
    //      std::uint32_t m_sent_bytes;
};

}  // namespace sim
