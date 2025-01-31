#pragma once

#include <cstdint>
#include <functional>

class Event {
public:
    Event(std::uint64_t time, const std::function<void()>& handler);

    void Execute() const;

    std::uint64_t execution_time;

    bool operator<(const Event& other) const;
private:
    std::function<void()> m_handler;
};
