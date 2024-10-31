#include "utils.hpp"

#include <stdexcept>

ServerBase::ServerBase(std::uint32_t id) : id(id) { }

std::uint32_t ServerBase::GetID() const {
    return id;
}

ServerSender::ServerSender(std::uint32_t id, std::uint32_t distance_us) 
    : ServerBase(id), distance_us(distance_us), cwnd(CWND()) { }                                   

std::uint32_t ServerSender::GetDistance() const {
    return distance_us;
}

std::uint32_t ServerSender::GetACKsNumber() const {
    return ACKs.size();
}

std::uint32_t ServerSender::GetCWNDSize() const {
    return cwnd.GetSize();
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
    cwnd.IncreaseSize();
}

void ServerSender::AddACK(const PacketHeader& packet) {
    ACKs.push_back(packet);
}

void ServerSender::printACKs() const {
    for (const auto& ack: ACKs) {
        std::cout << ack.GetEstimatedDeliveryTime() << std::endl;
    }
}

PacketHeader::PacketHeader(std::uint64_t sender_id, std::uint64_t receiver_id, std::uint64_t time, std::uint32_t index, std::uint32_t trip_time)
    : sender_id(sender_id), receiver_id(receiver_id), sending_time(time), packet_index(index), estimated_delivery_time(trip_time) { }

std::uint32_t PacketHeader::GetEstimatedDeliveryTime() const {
    return estimated_delivery_time;
}

std::uint64_t PacketHeader::GetSendingTime() const {
    return sending_time;
}

std::uint32_t PacketHeader::GetPacketIndex() const {
    return packet_index;
}

std::uint64_t PacketHeader::GetSenderID() const {
    return sender_id;
}

// The shorter the approx_trip_time, the faster the packet will delivered
bool PacketHeader::operator<(const PacketHeader &other) const {
    return estimated_delivery_time > other.GetEstimatedDeliveryTime();
}

Event::Event(ServerBase& initiator, event_type type, std::uint32_t packets_number)
    : server_initiator(initiator), type(type), packets_number(packets_number) { }

std::ostream& operator<<(std::ostream& out, const Event& event) {
    out << "[EVENT] ID: "
        << ". Initiator ID: " << event.server_initiator.GetID()
        << ". Type: " << (event.type == 0 ? "SEND_DATA" : "ACKNOWLEDGEMENT") 
        << ". Number of packets: " << event.packets_number << ".\n";
    
    return out;
}

void CWND::IncreaseSize() {
    if (size_in_packets * 2 < ssthresh) {
        // Slow start
        size_in_packets *= 2;
    }
    else {
        // ssthresh reached, increase size by one packet
        size_in_packets++;
    }
}

std::uint32_t CWND::GetSize() const {
    return size_in_packets;
}