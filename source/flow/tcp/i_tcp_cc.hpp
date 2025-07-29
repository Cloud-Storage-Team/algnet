#pragma once
#include "types.hpp"

namespace sim {

// Interfacce of TCP-like congestion control (CC) module
class ITcpCC {
public:
    // Callback that triggers every time ACK receives on sender
    // returns true if congestion detected; false otherwice
    virtual bool on_ack(TimeNs rtt, TimeNs avg_rtt, bool ecn_flag) = 0;
    virtual TimeNs get_pacing_delay() const = 0;
    virtual double get_cwnd() const = 0;
    virtual std::string to_string() const = 0;
};
}  // namespace sim
