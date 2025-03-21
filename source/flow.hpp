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
    virtual void update(std::uint32_t delay) = 0;

    virtual ISender* get_sender() const noexcept = 0;
    virtual IReceiver* get_receiver() const noexcept = 0;

    virtual ~IFlow() = default;
};

class Flow : public IFlow {  // Fixed inheritance
public:
    Flow(ISender* a_src, IReceiver* a_dest, uint32_t a_packet_size);
    void start(std::uint32_t time) override = 0;
    bool try_to_generate(std::uint32_t packet_size) override = 0;
    void update(std::uint32_t delay) override = 0;

    ISender* get_sender() const noexcept override;
    IReceiver* get_receiver() const noexcept override;

    ~Flow() override = default;

protected:
    void schedule_packet_generation(std::uint32_t time);
    void generate_packet();

private:
    ISender* m_src;
    IReceiver* m_dest;
    std::uint32_t m_packet_size;
};

}  // namespace sim
