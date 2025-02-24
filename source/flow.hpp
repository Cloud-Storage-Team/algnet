#pragma once
#include <cstdint>

#include "device.hpp"

namespace sim {

class Flow {
public:
    virtual ~Flow() = default;

    Flow(Device* a_src, Device* a_dest);

    // Start at time
    virtual void start(std::uint32_t time);

    // Try to generate a new packet if the internal state allows to do so.
    // Returns true if a generate_event was created and placed in scheduler
    virtual bool try_to_generate(std::uint32_t current_time) = 0;

    // Update flow state based on delay
    virtual void update(std::uint32_t delay) = 0;

    Device* m_src;
    Device* m_dest;

protected:
    void schedule_packet_generation(std::uint32_t time);
    std::uint32_t m_packet_size;
};

}  // namespace sim
