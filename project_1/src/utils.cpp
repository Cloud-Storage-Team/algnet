#include "utils.hpp"

#include <stdexcept>

TransmissionUnit::TransmissionUnit(std::uint64_t time)
    : estimated_delivery_time(time) { }

ACK::ACK(std::uint64_t receiver_id, std::uint64_t time)
    : TransmissionUnit(time), receiver_id(receiver_id) { }

std::uint64_t ACK::GetEstimatedDeliveryTime() const {
    return estimated_delivery_time;
}

std::uint64_t ACK::GetReceiverID() const {
    return receiver_id;
}

ServerBase::ServerBase(std::uint32_t id)
    : id(id) { }

std::uint32_t ServerBase::GetID() const {
    return id;
}

ServerSender::ServerSender(std::uint32_t id, std::uint32_t distance_us) 
    : ServerBase(id), distance_us(distance_us) { }

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

void ServerSender::UpdateCurrentTime(std::uint32_t new_time) {
    if (new_time > current_time_us) {
        current_time_us = new_time;
    }
    else {
        throw std::runtime_error("Incorrect value of last packet sending time.");
    }
}

void ServerSender::IncreaseCurrentTime(std::uint32_t time_step) {
    current_time_us += time_step;
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
    cwnd_size_in_packets = std::min(static_cast<float>(cwnd_size_in_packets + ACKs.size()),
                                    static_cast<float>(max_transmission_rate_packets));
    ACKs.clear();
    return true;
}

PacketHeader::PacketHeader(std::uint64_t sender_id, std::uint64_t time, std::uint32_t trip_time)
    : TransmissionUnit(trip_time), sender_id(sender_id), sending_time(time) { }

std::uint32_t PacketHeader::GetEstimatedDeliveryTime() const {
    return estimated_delivery_time;
}

std::uint64_t PacketHeader::GetSendingTime() const {
    return sending_time;
}

std::uint64_t PacketHeader::GetSenderID() const {
    return sender_id;
}

// The shorter the approx_trip_time, the faster the packet will delivered
bool PacketHeader::operator<(const PacketHeader &other) const {
    return estimated_delivery_time > other.GetEstimatedDeliveryTime();
}

Event::Event(ServerBase& initiator, event_type type, std::uint32_t units_number)
    : server_initiator(initiator), type(type), units_number(units_number) { }

std::ostream& operator<<(std::ostream& out, const Event& event) {
    out << "Initiator ID: " << event.server_initiator.GetID();
    if (!event.type) {
        out << ". Type: SEND_DATA"
            << ". Number of packets: " << event.units_number << ".\n";
    }
    else {
        out << ". Type: ACKNOWLEDGEMENT.\n";
    }
    return out;
}