#pragma once

#include <cstdint>
#include <iostream>
#include <chrono>

std::uint64_t GetCurrentTimeInMicroseconds();

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
    std::uint32_t distance_μs;
public:
    ServerSender(std::uint32_t, std::uint32_t);
    ServerSender() = default;
    ~ServerSender() = default;

    std::uint32_t GetDistance() const;
};

// Type for package
struct PackageHeader {
private:
    std::uint64_t sender_id;
    std::uint64_t receiver_id;
    std::uint64_t sending_time;
    std::uint32_t package_index;
    std::uint32_t RTT;
public:
    PackageHeader() = default;
    PackageHeader(std::uint64_t, std::uint64_t, std::uint64_t, std::uint32_t);

    void SetRTT(std::uint32_t);
    std::uint32_t GetRTT() const;
    std::uint64_t GetSendingTime() const;
    std::uint32_t GetPackageIndex() const;
    std::uint64_t GetSenderID() const;

    bool operator<(const PackageHeader& other) const;
};

// Struct represents event -- sending data or acknowledgement
struct Event {
private:
    std::uint64_t id;
    ServerBase& server_initiator;
    event_type type;

    // number of sending packages
    std::uint32_t packages_number;  
public:
    Event(std::uint64_t, ServerBase&, event_type, std::uint32_t);
    ~Event() = default;

    std::uint32_t GetID() const;

    friend std::ostream& operator<<(std::ostream& out, const Event& event);
};

