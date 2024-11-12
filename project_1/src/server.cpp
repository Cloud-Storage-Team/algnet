#include "server.hpp"

#include <iostream>

ServerBase::ServerBase(std::uint32_t id)
    : id(id) { }

std::uint32_t ServerBase::GetID() const {
    return id;
}

ServerSender::ServerSender(std::uint32_t id, std::uint32_t distance_us, std::priority_queue<PacketHeader>& queue) 
    : ServerBase(id), distance_us(distance_us), packet_queue(queue) { }

std::uint32_t ServerSender::GetDistance() const {
    return distance_us;
}

std::uint32_t ServerSender::GetACKsNumber() const {
    return ACKs.size();
}

std::uint32_t ServerSender::GetCWNDSize() const {
    return cwnd_size_in_packets;
}

std::uint32_t ServerSender::GetCurrentTime() const {
    return current_time_us;
}

void ServerSender::UpdateCurrentTime(std::uint32_t new_time_us) {
    // Situation when new_time is less than current_time_us is possible
    current_time_us = std::max<std::uint32_t>(current_time_us, new_time_us);
}

void ServerSender::IncreaseCurrentTime(std::uint32_t delta_time_us) {
    current_time_us += delta_time_us;
}

void ServerSender::IncreaseCWNDSize() {
    ++cwnd_size_in_packets;
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
    cwnd_size_in_packets = std::min<std::uint32_t>(cwnd_size_in_packets + ACKs.size(), max_transmission_rate_packets);
    // Sender's current time is the delivery time of the last ACK
    UpdateCurrentTime(ACKs.back().GetEstimatedDeliveryTime());
    ACKs.clear();
    return true;
}

void ServerSender::SendPackets() {
    // Create packets, add them to priority_queue
    for (std::uint32_t i = 0; i < cwnd_size_in_packets; ++i) {
        // Calculate estimated delivering time of the packet
        std::uint64_t estimated_delivering_time = current_time_us + distance_us;
        // Add packet to the priority_queue
        packet_queue.emplace(id, current_time_us, estimated_delivering_time);

        // Update server time
        IncreaseCurrentTime(distance_us);
    }
}