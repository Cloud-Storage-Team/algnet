#pragma once

#include <cstdint>
#include <iostream>
#include <vector>

enum event_type: std::uint8_t {
    SEND_DATA,
    ACKNOWLEDGEMENT,
};

// Type for packet
struct PacketHeader {
private:
    std::uint64_t sender_id;
    std::uint64_t receiver_id;
    std::uint64_t sending_time;
    std::uint32_t packet_index;
    std::uint32_t estimated_delivery_time;
public:
    PacketHeader() = default;
    PacketHeader(std::uint64_t, std::uint64_t, std::uint64_t, std::uint32_t, std::uint32_t);

    std::uint32_t GetEstimatedDeliveryTime() const;
    std::uint64_t GetSendingTime() const;
    std::uint32_t GetPacketIndex() const;
    std::uint64_t GetSenderID() const;

    bool operator<(const PacketHeader& other) const;
};

// Type for congestion window
struct CWND {
private:
    std::uint32_t size_in_packets = 8;

    // Slow-start threshold in packets
    std::uint32_t ssthresh = 1024;
public:
    CWND() = default;

    std::uint32_t GetSize() const;
    void IncreaseSize();
};

// Type for server receiver, contains only ID
struct ServerBase {
protected:
    std::uint64_t id;
public:
    ServerBase(std::uint32_t);
    ServerBase() = default;
    ~ServerBase() = default;

    std::uint32_t GetID() const;
};

// Type for sending server, contains ID and distance to receiver
struct ServerSender : ServerBase {
private:
    std::uint32_t distance_us;
    std::uint32_t current_time_us = 0;
    CWND cwnd;

    std::vector<PacketHeader> ACKs{};
public:
    ServerSender(std::uint32_t, std::uint32_t);
    ServerSender() = default;
    ~ServerSender() = default;

    std::uint32_t GetDistance() const;
    std::uint32_t GetACKsNumber() const;
    std::uint32_t GetCWNDSize() const;
    std::uint32_t GetCurrentTime() const;
    void UpdateCurrentTime(std::uint32_t);
    void IncreaseCurrentTime(std::uint32_t);
    void IncreaseCWNDSize();
    void AddACK(const PacketHeader&);

    void printACKs() const;
};

// Struct represents event -- sending data or acknowledgement
struct Event {
private:
    ServerBase& server_initiator;
    event_type type;

    // number of sending packets
    std::uint32_t packets_number;  
public:
    Event(ServerBase&, event_type, std::uint32_t);
    ~Event() = default;

    friend std::ostream& operator<<(std::ostream& out, const Event& event);
};

