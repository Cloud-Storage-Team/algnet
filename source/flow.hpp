#pragma once
#include <cstdint>

#include "receiver.hpp"
#include "sender.hpp"

namespace sim {

class IFlow {
    public:
        // Start at time
        virtual void start(std::uint32_t time) = 0;

        // Try to generate a new packet if the internal state allows to do so.
        // Returns true if a generate_event was created and placed in scheduler
        virtual bool try_to_generate(std::uint32_t packet_size) = 0;
    
        // Update the internal state according to some congestion control algorithm
        // Call try_to_generate upon the update
        virtual void update() = 0;

        virtual ISender* get_sender() const noexcept = 0;
        virtual IReceiver* get_receiver() const noexcept = 0;
    
        virtual ~IFlow() = default;
    };

class Flow : IFlow {
public:
    Flow(ISender *a_src, IReceiver *a_dest, uint32_t a_packet_size);
    virtual void start(std::uint32_t time) = 0;
    virtual bool try_to_generate(std::uint32_t current_time) = 0;
    virtual void update(std::uint32_t delay) = 0;

    ISender *m_src;
    IReceiver *m_dest;

    virtual ~Flow() = default;
protected:
    void schedule_packet_generation(std::uint32_t time);
    void generate_packet();
    std::uint32_t m_packet_size;
};

}  // namespace sim
