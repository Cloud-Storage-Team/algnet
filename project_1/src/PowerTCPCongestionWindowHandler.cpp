#include "PowerTCPCongestionWindowHandler.hpp"

#include <cmath>
#include <iostream>

PowerTCPCongestionWindowHandler::PowerTCPCongestionWindowHandler(std::uint64_t base_rtt, std::uint64_t cwnd_init, double gamma, double beta)
    : CongestionWindowHandler(base_rtt, cwnd_init), gamma(gamma), beta(beta), rate(cwnd_init / base_rtt) {

    cwnd_by_ts[0] = cwnd_init;
}

void PowerTCPCongestionWindowHandler::updateOnPacket(const TransmissionUnit &packet, std::uint64_t ts) {
    std::uint32_t cwnd_old = getCWNDOld(!packet.headers.empty() ? packet.headers.front().timestamp : 0);
#ifdef DEBUG
    std::cout << "CWND old: " << cwnd_old << "\n";
#endif
    double normalized_power = calculateNormalizedPower(packet);
#ifdef DEBUG
    std::cout << "Normalized power: " << normalized_power << "\n";
#endif
    rate = updateWindow(normalized_power, cwnd_old) / base_rtt;
#ifdef DEBUG
    std::cout << "Rate: " << rate << "\n";
#endif
    prev_headers = packet.headers;
    cwnd_by_ts[ts] = cwnd;
}

std::uint64_t PowerTCPCongestionWindowHandler::getCWND() const {
    return cwnd;
}

std::uint64_t PowerTCPCongestionWindowHandler::getRate() const {
    return rate;
}

std::uint64_t PowerTCPCongestionWindowHandler::getCWNDOld(std::uint64_t ts) {
    auto cwnd_it = cwnd_by_ts.upper_bound(ts);
    return (--cwnd_it)->second;
}

double PowerTCPCongestionWindowHandler::calculateNormalizedPower(const TransmissionUnit &packet) {
    if (prev_headers.empty()) {
        prev_headers.resize(packet.headers.size(), {});
    }

    double max_normalized_power = 0;
    std::uint64_t delta_t = 0;
    for (size_t i = 0; i < packet.headers.size(); ++i) {
        const auto &header = packet.headers[i];
        const auto &prev_header = prev_headers[i];

        std::uint64_t dt = header.timestamp - prev_header.timestamp;
        double q_len_rate = double(header.queue_length - prev_header.queue_length) / dt;
        double tx_rate = double(header.transmitted_bytes - prev_header.transmitted_bytes) / dt;
        double current = q_len_rate + tx_rate;
        std::uint64_t bdp = header.bandwidth_capacity * base_rtt;
        std::uint64_t voltage = header.queue_length + bdp;
        double power = current * voltage;

        std::uint64_t base_power = header.bandwidth_capacity * header.bandwidth_capacity * base_rtt;
        double normalized_power = power / base_power;
        if (normalized_power > max_normalized_power) {
            max_normalized_power = normalized_power;
            delta_t = dt;
        }
    }
    smoothed_power = (smoothed_power * (base_rtt - delta_t) + max_normalized_power * delta_t) / base_rtt;
    return smoothed_power;
}

std::uint64_t PowerTCPCongestionWindowHandler::updateWindow(double normalized_power, std::uint64_t cwnd_old) {
    cwnd = std::floor(gamma * (double(cwnd_old) / std::max(normalized_power, 1.0) + beta) + (1 - gamma) * cwnd);
    return cwnd; 
}