#pragma once

#include "CongestionWindowHandler.hpp"

#include <cstdint>
#include <vector>
#include <map>

class PowerTCPCongestionWindowHandler : public CongestionWindowHandler {
public:
    // Advised values: gamma = 0.8, beta = (host_bandwidth * _base_rtt) / number_of_flows, cwnd_init = (host_bandwidth * _base_rtt)
    PowerTCPCongestionWindowHandler(std::uint64_t base_rtt, std::uint64_t cwnd_init, double gamma, double beta);

    void updateOnPacket(const TransmissionUnit &packet, std::uint64_t ts) override;

    std::uint64_t getCWND() const override;

    std::uint64_t getRate() const override;

private:
    std::uint64_t getCWNDOld(std::uint64_t ts);

    double calculateNormalizedPower(const TransmissionUnit &packet);

    std::uint64_t updateWindow(double normalized_power, std::uint64_t cwnd_old);

    double gamma;
    double beta;
    std::uint64_t rate;

    std::map<std::uint32_t, std::uint64_t> cwnd_by_ts;
    std::vector<INTHeader> prev_headers{};
    double smoothed_power = 0;
};