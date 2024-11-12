#pragma once

#include <cstdint>

struct TransmissionUnit {
    explicit TransmissionUnit(std::uint64_t delivery_time_us);
protected:
    std::uint64_t estimated_delivery_time;
};

// Type for acknowledgement
struct ACK : TransmissionUnit {
    ACK(std::uint64_t receiver_id, std::uint64_t delivery_time_us);
    std::uint64_t GetReceiverID() const;
    std::uint64_t GetEstimatedDeliveryTime() const;
private:
    std::uint64_t receiver_id;
};

// Type for packet
struct PacketHeader : TransmissionUnit {
    PacketHeader(std::uint64_t sender_id, std::uint64_t sending_time_us, std::uint32_t delivery_time_us);
    //PacketHeader() = default;
    std::uint32_t GetEstimatedDeliveryTime() const;
    std::uint64_t GetSendingTime() const;
    std::uint64_t GetSenderID() const;

    bool operator<(const PacketHeader& other) const;
private:
    std::uint64_t sender_id;
    std::uint64_t sending_time;
};