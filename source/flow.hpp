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

class Flow final : public IFlow, public std::enable_shared_from_this<Flow> {
public:
    Flow(ISender *a_src, IReceiver *a_dest, float a_start_cwnd) {
        (void)a_src;
        (void)a_dest;
        (void)a_start_cwnd;
    }

    // Start at time
    void start(Time time) final {
        (void)time;
        (void)m_src;
        (void)m_dest;
        (void)m_nacks;
        (void)m_cwnd;
        (void)m_sent_bytes;
    }

    // Try to generate a new packet if the internal state allows to do so.
    // by placing it into the flow buffer of the source node.
    // Schedule the next generation event.
    bool try_to_generate(Size packet_size) final {
        (void)packet_size;
        return false;
    }

    // Update the internal state according to some congestion control algorithm
    // Call try_to_generate upon the update
    void update() final {}

    std::shared_ptr<ISender> get_sender() const final { return nullptr; }
    std::shared_ptr<IReceiver> get_receiver() const final { return nullptr; }

private:
    ISender *m_src;
    IReceiver *m_dest;
    std::uint32_t m_nacks;
    float m_cwnd;
    Size m_sent_bytes;
};

}  // namespace sim
