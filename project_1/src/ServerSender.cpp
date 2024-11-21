#include "ServerSender.hpp"
#include "NetworkSimulator.hpp"

#include <iostream>

const std::uint64_t gbps_to_bps = 125'000'000;
const std::uint64_t one_sec_in_ns = 1'000'000'000;

std::uint32_t calcPacketTransmissionTime(std::uint64_t data_rate_gbps) {
    return static_cast<std::uint32_t>(one_sec_in_ns / (data_rate_gbps * gbps_to_bps / NetworkSimulator::packet_size_bytes));
}

std::uint32_t castGbpsToPackets(std::uint32_t data_rate_gbps) {
    return static_cast<std::uint32_t>(data_rate_gbps * gbps_to_bps / NetworkSimulator::packet_size_bytes);
}

ServerSender::ServerSender(std::uint64_t id, std::uint32_t data_rate_gbps)
    : ServerBase(id), max_data_rate_gbps(data_rate_gbps), max_data_rate_packets(castGbpsToPackets(data_rate_gbps)),
      packet_transmission_time(calcPacketTransmissionTime(data_rate_gbps)) { }


std::uint32_t ServerSender::GetDataRate() const {
    return max_data_rate_gbps;
}

std::uint32_t ServerSender::GetACKsNumber() const {
    return ACKs.size();
}

std::uint32_t ServerSender::GetPacketTransmissionTime() const {
    return packet_transmission_time;
}

std::uint32_t ServerSender::GetCWNDSize() const {
    return cwnd_size_in_packets;
}

std::uint32_t ServerSender::GetCurrentTime() const {
    return current_time_ns;
}

void ServerSender::UpdateCurrentTime(std::uint64_t new_time_ns) {
    // Situation when new_time is less than current_time_us is possible
    current_time_ns = std::max<std::uint64_t>(current_time_ns, new_time_ns);
}

void ServerSender::IncreaseCurrentTime(std::uint64_t delta_time_ns) {
    current_time_ns += delta_time_ns;
}

void ServerSender::IncreaseCWNDSize() {
    if (cwnd_size_in_packets < max_data_rate_packets) {
        ++cwnd_size_in_packets;
    }
}

void ServerSender::AddACK(const ACK& ack) {
    ACKs.push_back(ack);
}

void ServerSender::printACKs() const {
    for (const auto& ack: ACKs) {
        std::cout << ack.GetEstimatedDeliveryTime() << std::endl;
    }
}

bool ServerSender::HandleACKs() {
    if (ACKs.empty()) {
        return false;
    }
    cwnd_size_in_packets = std::min<std::uint32_t>(cwnd_size_in_packets + ACKs.size(), max_data_rate_packets);
    // Sender's current time is the delivery time of the last ACK
    UpdateCurrentTime(ACKs.back().GetEstimatedDeliveryTime());
    ACKs.clear();
    return true;
}

void ServerSender::SendPackets(std::priority_queue<PacketHeader>& packet_queue) {
    // Create packets, add them to priority_queue
    for (std::uint32_t i = 0; i < cwnd_size_in_packets; ++i) {
        // Calculate estimated delivering time of the packet
        std::uint64_t estimated_delivering_time = current_time_ns + packet_transmission_time;
        // Add packet to the priority_queue
        packet_queue.emplace(id, current_time_ns, estimated_delivering_time);
        // Update server time
        IncreaseCurrentTime(packet_transmission_time);
    }
}