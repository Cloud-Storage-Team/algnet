#pragma once
#include "connection/flow/tcp/i_tcp_cc.hpp"

namespace test {
class CCMock : public sim::ITcpCC {
public:
    CCMock(double a_cwnd, TimeNs a_pacing_delay)
        : m_cwnd(a_cwnd), m_pacing_delay(a_pacing_delay) {}

    virtual void on_ack([[maybe_unused]] TimeNs rtt,
                        [[maybe_unused]] TimeNs avg_rtt,
                        [[maybe_unused]] bool ecn_flag) {}

    // Callback that triggers when ACK not delivered on timeout
    virtual void on_timeout() {};

    // Inter‑packet pacing gap when cwnd < 1; zero otherwise
    virtual TimeNs get_pacing_delay() const { return m_pacing_delay; }

    // Current congestion window (in packets)
    virtual double get_cwnd() const { return m_cwnd; }

    // For logs
    virtual std::string to_string() const { return ""; }

    double& get_mutable_cwnd() { return m_cwnd; }

    TimeNs& set_mutable_pacing_delay() { return m_pacing_delay; }

private:
    double m_cwnd;
    TimeNs m_pacing_delay;
};
}  // namespace test
