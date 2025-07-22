#pragma once
#include "flow/tcp/i_tcp_cc.hpp"

namespace sim {
class TcpTahoeCC : public ITcpCC {
public:
    TcpTahoeCC(Time a_dealay_threshold = 4000, double a_sstresh = 8);
    ~TcpTahoeCC() = default;

    bool on_ack(Time rtt, double avg_rtt, bool ecn_flag) final;
    Time get_pacing_delay() const final;
    double get_cwnd() const final;
    std::string to_string() const final;

private:
    Time m_delay_threshold;  // delay threshold for update

    double m_ssthresh;  // Slow start threshold
    double m_cwnd;      // Congestion window
    Time m_last_congestion_detected;
};
}  // namespace sim
