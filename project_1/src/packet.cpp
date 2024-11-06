#include "packet.hpp"

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

// The earlier the estimated_delivery_time, the faster the packet will arrive
bool PacketHeader::operator<(const PacketHeader &other) const {
    return estimated_delivery_time > other.GetEstimatedDeliveryTime();
}