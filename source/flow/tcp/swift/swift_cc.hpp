#pragma once
// Interface of Google Swift congestion‑control
//
// Copyright 2025

#include <string>

#include "flow/tcp/i_tcp_cc.hpp"

namespace sim {

/**
 * TcpSwiftCC
 *
 * Self‑contained Swift congestion‑control for discrete‑event simulation.
 * Implements:
 *   • dynamic target‑delay (base + flow‑scaling) §3.1;
 *   • AIMD with at‑most‑once‑per‑RTT multiplicative decrease;
 *   • pacing when cwnd < 1 pkt.
 *
 * Hop‑based scaling is omitted until the simulator exposes TTL on ACKs.
 */
class TcpSwiftCC : public ITcpCC {
public:
    static constexpr double DEFAULT_START_CWND = 1.0;
    static constexpr double DEFAULT_AI = 0.5;
    static constexpr double DEFAULT_MD = 0.5;
    static constexpr double DEFAULT_MAX_MDF = 0.3;
    static constexpr double DEFAULT_FS_RANGE = 0.3;
    static constexpr double DEFAULT_FS_MIN_CWND = 0.1;   // taken from the paper
    static constexpr double DEFAULT_FS_MAX_CWND = 100.;  // taken from the paper
    /**
     * a_base_target      Minimal (fabric link with 1 hop) RTT, ns.
     * a_start_cwnd       Initial value of cwnd
     * a_ai               Additive‑increase constant.
     * a_md               β for multiplicative decrease.
     * a_max_mdf          Upper bound on MD factor per RTT.
     * a_fs_range         Max queue head‑room (flow‑scaling term).
     * a_fs_min_cwnd      cwnd where flow‑scaling hits fs_range.
     * a_fs_max_cwnd      cwnd where flow‑scaling goes to zero.
     */
    TcpSwiftCC(TimeNs a_base_target, double a_start_cwnd = DEFAULT_START_CWND,
               double a_ai = DEFAULT_AI,
               double a_md = DEFAULT_MD,  // from [0.2, 0.5] diapason
               double a_max_mdf = DEFAULT_MAX_MDF,
               double a_fs_range = DEFAULT_FS_RANGE,
               double a_fs_min_cwnd = DEFAULT_FS_MIN_CWND,
               double a_fs_max_cwnd = DEFAULT_FS_MAX_CWND);

    void on_ack(TimeNs rtt, TimeNs avg_rtt, bool ecn_flag) final;
    void on_timeout() final;

    [[nodiscard]] TimeNs get_pacing_delay() const final;

    [[nodiscard]] double get_cwnd() const final;

    [[nodiscard]] std::string to_string() const final;

    // TO‑DO: fast‑recovery hook

private:
    // Compute Swift target‑delay = base + flow‑scaling term.
    [[nodiscard]] TimeNs compute_target_delay() const;

    // Computes if cwnd might be decreased now
    // Based on m_last_rtt
    [[nodiscard]] bool compute_can_decrease() const;

    void update_cwnd(double neww_cwnd);

    // ---------- Tunables ----------
    const TimeNs m_base_target;  // base RTT budget (ns)
    const double m_ai;           // additive‑increase constant
    const double m_beta_md;      // β for multiplicative decrease
    const double m_max_mdf;      // cap on MD factor per RTT

    // Flow‑scaling parameters (cf. Swift §3.1)
    const TimeNs m_fs_range_ns;  // scale range ns (base target * fs range)
    const double m_fs_min_cwnd;  // packets
    const double m_fs_max_cwnd;  // packets
    TimeNs m_alpha_flow;         // flow scaling α, ns
    TimeNs m_beta_flow;          // flow scaling β (runtime‑set), ns
                         // a positive value (unlike the article), as the
                         // internal TimeNs type is used (not negative)
    static constexpr std::size_t M_RETX_RESET_THRESHOLD = 3;
    // ---------- СС state ----------
    double m_cwnd;           // congestion window (packets)
    TimeNs m_last_decrease;  // last MD timestamp
    TimeNs m_last_rtt;       // last sampled RTT
    std::size_t m_retransmit_cnt;

    // ----------- Limits -----------
    static constexpr double M_MIN_CWND = 0.001;  // taken from the paper
    static constexpr double M_MAX_CWND = 1e6;
};

}  // namespace sim
