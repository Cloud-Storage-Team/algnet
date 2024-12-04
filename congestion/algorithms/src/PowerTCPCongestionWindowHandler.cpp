#include "PowerTCPCongestionWindowHandler.h"

#include <cmath>
#include <iostream>

namespace congestion::algorithms {

PowerTCPCongestionWindowHandler::PowerTCPCongestionWindowHandler(std::uint32_t base_rtt, double gamma, double beta, std::uint32_t cwnd_init)
    : _base_rtt(base_rtt), _gamma(gamma), _beta(beta), _cwnd(cwnd_init), _rate(cwnd_init / base_rtt) {
    
    _cwnd_by_ts[0] = cwnd_init;
}

void PowerTCPCongestionWindowHandler::updateOnPacket(const Packet &packet, std::uint32_t ts) {
    std::uint32_t cwnd_old = getCWNDOld(packet.ts_send);
#ifdef DEBUG
    std::cout << "CWND old: " << cwnd_old << "\n";
#endif
    double normalized_power = calculateNormalizedPower(packet);
#ifdef DEBUG
    std::cout << "Normalized power: " << normalized_power << "\n";
#endif
    _rate = updateWindow(normalized_power, cwnd_old) / _base_rtt;
#ifdef DEBUG
    std::cout << "Rate: " << rate << "\n";
#endif
    _prev_headers = packet.headers;
    _cwnd_by_ts[ts] = _cwnd;
}

std::uint32_t PowerTCPCongestionWindowHandler::getCWND() const {
    return _cwnd;
}

std::uint32_t PowerTCPCongestionWindowHandler::getRate() const {
    return _rate;
}

std::uint32_t PowerTCPCongestionWindowHandler::getCWNDOld(std::uint32_t ts) {
    auto cwnd_it = _cwnd_by_ts.upper_bound(ts);
    return (--cwnd_it)->second;
}

double PowerTCPCongestionWindowHandler::calculateNormalizedPower(const Packet &packet) {
    if (_prev_headers.empty()) {
        _prev_headers.resize(packet.headers.size(), {});
    }

    double max_normalized_power = 0;
    std::uint32_t delta_t = 0;
    for (size_t i = 0; i < packet.headers.size(); ++i) {
        const auto &header = packet.headers[i];
        const auto &prev_header = _prev_headers[i];

        std::uint32_t dt = header.ts - prev_header.ts;
        double q_len_rate = double(header.q_len - prev_header.q_len) / dt;
        double tx_rate = double(header.tx_bytes - prev_header.tx_bytes) / dt;
        double current = q_len_rate + tx_rate;
        std::uint32_t bdp = header.b * _base_rtt;
        std::uint32_t voltage = header.q_len + bdp;
        double power = current * voltage;

        std::uint32_t base_power = header.b * header.b * _base_rtt;
        double normalized_power = power / base_power;
        if (normalized_power > max_normalized_power) {
            max_normalized_power = normalized_power;
            delta_t = dt;
        }
    }
    _smoothed_power = (_smoothed_power * (_base_rtt - delta_t) + max_normalized_power * delta_t) / _base_rtt;
    return _smoothed_power;
}

std::uint32_t PowerTCPCongestionWindowHandler::updateWindow(double normalized_power, std::uint32_t cwnd_old) {
    _cwnd = std::floor(_gamma * (double(cwnd_old) / normalized_power + _beta) + (1 - _gamma) * _cwnd);
    return _cwnd; 
}

} // namespace congestion::algorithms