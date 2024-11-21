#include "ACK.hpp"

ACK::ACK(std::uint64_t receiver_id, std::uint64_t delivery_time_ns)
    : TransmissionUnit(delivery_time_ns), receiver_id(receiver_id) { }

std::uint64_t ACK::GetReceiverID() const {
    return receiver_id;
}