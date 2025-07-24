#pragma once
#include "flow/tcp/i_tcp_cc.hpp"

namespace sim {
class TcpTahoeCC : public ITcpCC {
public:
    TcpTahoeCC(TimeNs a_delay_threshold = TimeNs(4000), double a_ssthresh = 8);
    ~TcpTahoeCC() = default;

    bool on_ack(TimeNs rtt, TimeNs avg_rtt, bool ecn_flag) final;
    TimeNs get_pacing_delay() const final;
    double get_cwnd() const final;
    std::string to_string() const final;

private:
    TimeNs m_delay_threshold;  // delay threshold for update

    double m_ssthresh;  // Slow start threshold
    double m_cwnd;      // Congestion window
    TimeNs m_last_congestion_detected;
};
}  // namespace sim
