#include "packet.hpp"

TransmissionUnit::TransmissionUnit(std::uint64_t delivery_time_us)
    : estimated_delivery_time(delivery_time_us) { }

ACK::ACK(std::uint64_t receiver_id, std::uint64_t delivery_time_us)
    : TransmissionUnit(delivery_time_us), receiver_id(receiver_id) { }

std::uint64_t ACK::GetEstimatedDeliveryTime() const {
    return estimated_delivery_time;
}

std::uint64_t ACK::GetReceiverID() const {
    return receiver_id;
}

PacketHeader::PacketHeader(std::uint64_t sender_id, std::uint64_t sending_time_us, std::uint32_t delivery_time_us)
    : TransmissionUnit(delivery_time_us), sender_id(sender_id), sending_time(sending_time_us) { }

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