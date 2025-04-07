#pragma once
#include <cstdint>

#include "receiver.hpp"
#include "sender.hpp"

namespace sim {

class IReceiver;
class ISender;

class IFlow {
public:
    virtual void start(Time time) = 0;
    virtual bool try_to_generate(Size packet_size) = 0;

    // Update the internal state according to some congestion control algorithm
    // Call try_to_generate upon the update
    virtual void update() = 0;
    virtual std::shared_ptr<ISender> get_sender() const = 0;
    virtual std::shared_ptr<IReceiver> get_receiver() const = 0;
};

class Flow : IFlow {
public:
    Flow(ISender *a_src, IReceiver *a_dest, float a_start_cwnd);

    // Start at time
    void start(Time time) final;

    // Try to generate a new packet if the internal state allows to do so.
    // by placing it into the flow buffer of the source node.
    // Schedule the next generation event.
    bool try_to_generate(Size packet_size) final;

    // Update the internal state according to some congestion control algorithm
    // Call try_to_generate upon the update
    void update() final;

    std::shared_ptr<ISender> get_sender() const final;
    std::shared_ptr<IReceiver> get_receiver() const final;

private:
    ISender *m_src;
    IReceiver *m_dest;
    std::uint32_t m_nacks;
    float m_cwnd;
    Size m_sent_bytes;
};

}  // namespace sim
