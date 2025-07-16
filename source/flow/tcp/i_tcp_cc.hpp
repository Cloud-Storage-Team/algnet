#pragma once
#include "types.hpp"

namespace sim {

class ITcpCC {
public:
    // Callback that triggers every time ACK receives on sender
    // returns true if congestion experienced; false otherwice
    virtual bool on_ack(Time rtt, bool ecn_flag) = 0;
    virtual Time get_pacing_delay() const = 0;
    virtual double get_cwnd() const = 0;
    virtual std::string to_string() const = 0;
};
}  // namespace sim
