#pragma once

#include <memory>
#include <vector>

#include "flow/i_flow.hpp"
#include "packet.hpp"
#include "types.hpp"

namespace sim {

// Captures per-packet flow-level statistics used by MPLB logic
struct FlowSample {
    TimeNs ack_recv_time;
    TimeNs packet_sent_time;
    std::uint32_t packets_in_flight;
    double curr_cwnd;
    SpeedGbps delivery_rate;
    // add other metrics here as needed (e.g., RTT variance, ECN, etc.)
};

class IMPLB {
public:
    virtual void add_flow(std::shared_ptr<IFlow> flow) = 0;
    virtual void remove_flow(const std::shared_ptr<IFlow>& flow) = 0;
    virtual void notify_ack_received(const std::shared_ptr<IFlow>& flow, FlowSample metrics) = 0;
    virtual void notify_packet_sent(const std::shared_ptr<IFlow>& flow, FlowSample metrics) = 0;
    virtual std::shared_ptr<IFlow> select_flow() = 0;
};

}  // namespace sim
