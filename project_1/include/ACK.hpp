#pragma once

#include "TransmissionUnit.hpp"

// Type for acknowledgement
struct ACK : TransmissionUnit {
    ACK(std::uint64_t receiver_id, std::uint64_t delivery_time_us);
    std::uint64_t GetReceiverID() const;

private:
    std::uint64_t receiver_id;
};