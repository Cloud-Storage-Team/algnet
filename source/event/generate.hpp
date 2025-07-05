#pragma once
#include "event.hpp"
#include "flow/i_flow.hpp"

namespace sim {
/**
 * Generate a packet in the flow and enqueue it in
 * the source node ingress buffer for processing.
 * Schedule the next packet generation event.
 */
class Generate : public Event {
public:
    Generate(Time a_time, std::weak_ptr<IFlow> a_flow, Size a_packet_size);
    virtual ~Generate() = default;
    void operator()() final;

private:
    std::weak_ptr<IFlow> m_flow;
    Size m_packet_size;
};

}  // namespace sim