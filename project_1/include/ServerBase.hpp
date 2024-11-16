#pragma once

#include "TransmissionUnit.hpp"

#include <cstdint>
#include <vector>
#include <queue>

// Type for server receiver, contains only ID
// Base struct for ServerSender
struct ServerBase {
    explicit ServerBase(std::uint64_t id);
    ServerBase() = default;
    virtual ~ServerBase() = default;

    std::uint32_t GetID() const;
protected:
    std::uint64_t id;
};