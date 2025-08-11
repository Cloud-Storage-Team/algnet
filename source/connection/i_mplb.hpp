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
    SpeedGbps delivery_rate;
    // add other metrics here as needed (e.g., RTT variance, ECN, etc.)
};

class IMPLB {
public:
    // Adds a flow to the MPLB for consideration in load balancing
    virtual void add_flow(const std::shared_ptr<IFlow> &flow) = 0;
    // Removes a flow from the MPLB so it will no longer be selected
    virtual void remove_flow(const std::shared_ptr<IFlow>& flow) = 0;
    // Notifies the MPLB that a packet was confirmed for the given flow,
    // providing feedback metrics that can influence future flow selection
    virtual void notify_packet_confirmed(const std::shared_ptr<IFlow>& flow, FlowSample metrics) = 0;
    // Selects the next flow to send data on according to the MPLB
    virtual std::shared_ptr<IFlow> select_flow() = 0;
};

}  // namespace sim
