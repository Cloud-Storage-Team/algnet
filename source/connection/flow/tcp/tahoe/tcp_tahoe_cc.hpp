#pragma once
#include "connection/flow/tcp/i_tcp_cc.hpp"

namespace sim {
class TcpTahoeCC : public ITcpCC {
public:
    static constexpr double DEFAULT_START_CWND = 1.0;
    static constexpr double DEFAULT_SSTRESH = 8;

    explicit TcpTahoeCC(double a_start_cwnd = DEFAULT_START_CWND,
                        double a_ssthresh = DEFAULT_SSTRESH);
    ~TcpTahoeCC() = default;

    void on_ack(TimeNs rtt, TimeNs avg_rtt, bool ecn_flag) final;
    void on_timeout() final;
    TimeNs get_pacing_delay() const final;
    double get_cwnd() const final;
    std::string to_string() const final;

private:
    double m_ssthresh;  // Slow start threshold
    double m_cwnd;      // Congestion window
    TimeNs m_last_congestion_detected;
    TimeNs m_last_avg_rtt;
};
}  // namespace sim
