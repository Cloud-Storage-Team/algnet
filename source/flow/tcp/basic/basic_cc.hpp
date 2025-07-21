#pragma once
#include "flow/tcp/i_tcp_cc.hpp"

// Represents no congestion control
namespace sim {
class BasicCC : public ITcpCC {
public:
    bool on_ack(TimeNs rtt, bool ecn_flag) final;
    TimeNs get_pacing_delay() const final;
    double get_cwnd() const final;
    std::string to_string() const final;
};
}  // namespace sim