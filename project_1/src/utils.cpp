#include "utils.hpp"

ServerBase::ServerBase(std::uint32_t id) : id(id) { }

std::uint32_t ServerBase::GetID() const {
    return id;
}

ServerSender::ServerSender(std::uint32_t id, std::uint32_t distance_us) 
    : ServerBase(id), distance_us(distance_us) { }                                   

std::uint32_t ServerSender::GetDistance() const {
    return distance_us;
}

PacketHeader::PacketHeader(std::uint64_t sender_id, std::uint64_t receiver_id, std::uint64_t time, std::uint32_t index, std::uint32_t trip_time)
    : sender_id(sender_id), receiver_id(receiver_id), sending_time(time), packet_index(index), approx_trip_time(trip_time) { }

std::uint32_t PacketHeader::GetApproxTripTime() const {
    return approx_trip_time;
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
    return approx_trip_time > other.GetApproxTripTime();
}

Event::Event(std::uint64_t id, ServerBase& initiator, event_type type, std::uint32_t packets_number)
    : id(id), server_initiator(initiator), type(type), packets_number(packets_number) { }

std::uint32_t Event::GetID() const {
    return id;
}

std::ostream& operator<<(std::ostream& out, const Event& event) {
    out << "[EVENT] ID: " << event.id
        << ". Initiator ID: " << event.server_initiator.GetID()
        << ". Type: " << (event.type == 0 ? "SEND_DATA" : "ACKNOWLEDGEMENT") 
        << ". Number of packets: " << event.packets_number << ".\n";
    
    return out;
}