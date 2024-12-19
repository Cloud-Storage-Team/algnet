#pragma once

#include "Packet.hpp"

#include <memory>
#include <cstdint>
#include <functional>

class Event {
public:
    Event(std::uint64_t time, const std::function<void()>& handler);

    std::uint64_t GetTime() const;
    void Execute() const;

    std::uint64_t execution_time;
    std::function<void()> handler;

    bool operator<(const Event& other) const;
};