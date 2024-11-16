#include "ACK.hpp"

ACK::ACK(std::uint64_t receiver_id, std::uint64_t delivery_time_us)
        : TransmissionUnit(delivery_time_us), receiver_id(receiver_id) { }

std::uint64_t ACK::GetReceiverID() const {
    return receiver_id;
}