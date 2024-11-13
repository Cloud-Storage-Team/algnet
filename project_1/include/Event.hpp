#pragma once

#include "ServerBase.hpp"

#include <cstdint>
#include <iostream>

enum class event_type: std::uint8_t {
    SEND_DATA,
    ACKNOWLEDGEMENT,
};

// Struct represents event -- sending data or acknowledgement
struct Event {
    Event(ServerBase& initiator, event_type type, std::uint32_t units_number);
    virtual ~Event() = default;

    friend std::ostream& operator<<(std::ostream& out, const Event& event);
private:
    ServerBase& server_initiator;
    event_type type;

    // number of sending units
    std::uint32_t units_number;
};