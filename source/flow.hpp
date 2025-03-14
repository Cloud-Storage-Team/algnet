#pragma once
#include <cstdint>

#include "receiver.hpp"
#include "sender.hpp"

namespace sim {

class IFlow {
public:
    virtual ~IFlow() = default;

    IFlow(ISender *a_src, IReceiver *a_dest, uint32_t a_packet_size);

    // Start at time
    virtual void start(std::uint32_t time) = 0;

    // Try to generate a new packet if the internal state allows to do so.
    // Returns true if a generate_event was created and placed in scheduler
    virtual bool try_to_generate(std::uint32_t current_time) = 0;

    // Update flow state based on delay
    virtual void update(std::uint32_t delay) = 0;

    ISender *m_src;
    IReceiver *m_dest;

protected:
    void schedule_packet_generation(std::uint32_t time);
    void generate_packet();
    std::uint32_t m_packet_size;
};

}  // namespace sim
