#pragma once

#include "CongestionWindowHandler.h"

#include <cstdint>
#include <vector>
#include <map>

namespace congestion::algorithms {

class PowerTCPCongestionWindowHandler : public CongestionWindowHandler {
public:
    // Advised values: gamma = 0.8, beta = (host_bandwidth * _base_rtt) / number_of_flows, cwnd_init = (host_bandwidth * _base_rtt)
    PowerTCPCongestionWindowHandler(std::uint32_t base_rtt, double gamma, double beta, std::uint32_t cwnd_init);

    void updateOnPacket(const Packet &packet, std::uint32_t ts) override;

    std::uint32_t getCWND() const override;

    std::uint32_t getRate() const override;

private:
    std::uint32_t getCWNDOld(std::uint32_t ts);

    double calculateNormalizedPower(const Packet &packet);

    std::uint32_t updateWindow(double normalized_power, std::uint32_t cwnd_old);

    std::uint32_t _base_rtt;
    double _gamma;
    double _beta;
    std::uint32_t _cwnd;
    std::uint32_t _rate;

    std::map<std::uint32_t, std::uint32_t> _cwnd_by_ts;
    std::vector<INTHeader> _prev_headers{};
    double _smoothed_power = 0;
};

} // namespace congestion::algorithms