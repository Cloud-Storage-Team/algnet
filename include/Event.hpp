#pragma once

#include "Packet.hpp"

#include <memory>
#include <cstdint>
#include <functional>

class Event {
public:
    Event(double time, const std::function<void()>& handler);

    double GetTime() const;
    void Execute() const;

    double execution_time;
    std::function<void()> handler;

    bool operator<(const Event& other) const;
};