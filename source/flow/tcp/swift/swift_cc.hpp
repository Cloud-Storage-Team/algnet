#pragma once
// swift_cc.hpp
//
// Interface of Google Swift congestion‑control
//
// Copyright 2025

#include <string>

#include "flow/tcp/i_tcp_cc.hpp"
#include "scheduler.hpp"

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
    /**
     * base_target      Minimal (fabric link with 1 hop) RTT, ns.
     * a_additive_inc   Additive‑increase constant.
     * a_md_beta        β for multiplicative decrease.
     * a_max_mdf        Upper bound on MD factor per RTT.
     * fs_range         Max queue head‑room (flow‑scaling term).
     * fs_min_cwnd      cwnd where flow‑scaling hits fs_range.
     * fs_max_cwnd      cwnd where flow‑scaling goes to zero.
     */
    TcpSwiftCC(
        TimeNs  base_target,
        double  a_additive_inc   = 0.5,
        double  a_md_beta        = 0.5,  // from [0.2, 0.5] diapason
        double  a_max_mdf        = 0.3,
        double  fs_range         = 5.0,  // taken from the paper
        double  fs_min_cwnd      = 0.1,  // taken from the paper
        double  fs_max_cwnd      = 100.0 // taken from the paper
    );

    // Process one ACK; return true if cwnd was multiplicatively reduced.
    bool on_ack(TimeNs rtt_us, TimeNs avg_rtt, bool ecn_flag) override;

    // Inter‑packet pacing gap when cwnd < 1; zero otherwise.
    [[nodiscard]] TimeNs get_pacing_delay() const override;

    // Current congestion window (in packets).
    [[nodiscard]] double get_cwnd() const override;

    // For logs.
    [[nodiscard]] std::string to_string() const override;

    /* TO‑DO: timeout / fast‑recovery hooks
       void on_timeout();
       void on_fast_recovery();                                          */

private:
    // Compute Swift target‑delay = base + flow‑scaling term.
    [[nodiscard]] TimeNs compute_target_delay() const;

    // ---------- Tunables ----------
    const TimeNs m_base_target;   // base RTT budget (µs)
    const double m_ai;            // additive‑increase constant
    const double m_beta_md;       // β for multiplicative decrease
    const double m_max_mdf;       // cap on MD factor per RTT

    // Flow‑scaling parameters (cf. Swift §3.1)
    const double m_fs_range;      // scale range
    const double m_fs_min_cwnd;   // packets
    const double m_fs_max_cwnd;   // packets
    long double  m_alpha;         // derived coefficient α
    long double  m_beta_flow;     // reused as flow‑scaling β (runtime‑set)

    // ---------- СС state ----------
    double  m_cwnd;               // congestion window (packets)
    TimeNs  m_last_decrease;      // last MD timestamp
    TimeNs  m_last_rtt;           // last sampled RTT

    // ----------- Limits -----------
    static constexpr long double MIN_CWND =  0.001L;
    static constexpr double MAX_CWND =  1e6;
};

} // namespace sim
