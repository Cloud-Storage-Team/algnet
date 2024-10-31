#pragma once

#include <cstdint>
#include <iostream>

enum event_type: std::uint8_t {
    SEND_DATA,
    ACKNOWLEDGEMENT,
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
public:
    ServerSender(std::uint32_t, std::uint32_t);
    ServerSender() = default;
    ~ServerSender() = default;

    std::uint32_t GetDistance() const;
};

// Type for packet
struct PacketHeader {
private:
    std::uint64_t sender_id;
    std::uint64_t receiver_id;
    std::uint64_t sending_time;
    std::uint32_t packet_index;
    std::uint32_t approx_trip_time;
public:
    PacketHeader() = default;
    PacketHeader(std::uint64_t, std::uint64_t, std::uint64_t, std::uint32_t, std::uint32_t);

    std::uint32_t GetApproxTripTime() const;
    std::uint64_t GetSendingTime() const;
    std::uint32_t GetPacketIndex() const;
    std::uint64_t GetSenderID() const;

    bool operator<(const PacketHeader& other) const;
};

// Struct represents event -- sending data or acknowledgement
struct Event {
private:
    std::uint64_t id;
    ServerBase& server_initiator;
    event_type type;

    // number of sending packets
    std::uint32_t packets_number;  
public:
    Event(std::uint64_t, ServerBase&, event_type, std::uint32_t);
    ~Event() = default;

    std::uint32_t GetID() const;

    friend std::ostream& operator<<(std::ostream& out, const Event& event);
};

