#pragma once
#include "../packet_ack_info.hpp"
#include "types.hpp"

namespace sim {

// Interfacce of TCP-like congestion control (CC) module
class ITcpCC {
public:
    // Callback that triggers every time ACK receives on sender
    // returns true if congestion detected; false otherwice
    virtual void on_ack(TimeNs rtt, TimeNs avg_rtt, bool ecn_flag) = 0;

    virtual void on_ack(PacketAckInfo info) {
        on_ack(info.rtt, info.avg_rtt, info.ecn_flag);
    }

    // Callback that triggers when ACK not delivered on timeout
    virtual void on_timeout() = 0;

    // Inter‑packet pacing gap when cwnd < 1; zero otherwise
    virtual TimeNs get_pacing_delay() const = 0;

    // Current congestion window (in packets)
    virtual double get_cwnd() const = 0;

    // For logs
    virtual std::string to_string() const = 0;

    virtual ~ITcpCC() = default;
};
}  // namespace sim
