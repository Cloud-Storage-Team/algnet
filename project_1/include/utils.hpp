#pragma once

#include <cstdint>
#include <iostream>

enum event_type: std::uint8_t {
    SEND_DATA,
    ACKNOWLEDGEMENT,
};

struct Server {
private:
    std::uint32_t id;
    std::uint32_t distance_ms;
public:
    Server(std::uint32_t, std::uint32_t);
    Server() = default;
    ~Server() = default;

    std::uint32_t GetID() const;
    std::uint32_t GetDistance() const;
};

struct PackageHeader {
private:
    std::uint32_t sender_id;
    std::uint32_t receiver_id;
    std::uint32_t sending_time;
    std::uint32_t total_number_of_packages;
    std::uint32_t package_index;
    std::uint32_t RTT;
public:
    PackageHeader() = default;
    PackageHeader(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

    void SetRTT(std::uint32_t);
    std::uint32_t GetRTT() const;
    std::uint32_t GetSendingTime() const;
    std::uint32_t GetPackageIndex() const;
    std::uint32_t GetSenderID() const;

    bool operator<(const PackageHeader& other) const;
};

struct Event {
private:
    std::uint32_t id;
    Server& server_initiator;
    event_type type;
    std::uint32_t packages_number;
public:
    Event(std::uint32_t, Server&, event_type, std::uint32_t);
    ~Event() = default;

    std::uint32_t GetID() const;
};

