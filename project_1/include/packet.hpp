#pragma once

#include <cstdint>

struct TransmissionUnit {
protected:
    std::uint64_t estimated_delivery_time;
public:
    explicit TransmissionUnit(std::uint64_t delivery_time_us);
};

// Type for acknowledgement
struct ACK : TransmissionUnit {
private:
    std::uint64_t receiver_id;
public:
    ACK(std::uint64_t receiver_id, std::uint64_t delivery_time_us);

    std::uint64_t GetReceiverID() const;
    std::uint64_t GetEstimatedDeliveryTime() const;
};

// Type for packet
struct PacketHeader : TransmissionUnit {
private:
    std::uint64_t sender_id;
    std::uint64_t sending_time;
public:
    PacketHeader() = default;
    PacketHeader(std::uint64_t sender_id, std::uint64_t sending_time_us, std::uint32_t delivery_time_us);

    std::uint32_t GetEstimatedDeliveryTime() const;
    std::uint64_t GetSendingTime() const;
    std::uint64_t GetSenderID() const;

    bool operator<(const PacketHeader& other) const;
};