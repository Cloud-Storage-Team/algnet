#pragma once

#include "TransmissionUnit.hpp"

#include <cstdint>
#include <vector>

class CongestionWindowHandler {
public:
    CongestionWindowHandler(std::uint64_t base_rtt, std::uint64_t base_cwnd)
        : base_rtt(base_rtt), cwnd(base_cwnd) {}

    virtual ~CongestionWindowHandler() = default;

    virtual void updateOnPacket(const TransmissionUnit &packet, std::uint64_t timestamp) {}

    virtual std::uint64_t getCWND() const {
        return cwnd;
    }

    virtual std::uint64_t getRate() const {
        return cwnd / base_rtt;
    }

protected:
    std::uint64_t base_rtt;
    std::uint64_t cwnd;
};