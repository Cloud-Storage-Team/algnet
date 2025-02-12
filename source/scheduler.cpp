#include "scheduler.hpp"

#include <spdlog/spdlog.h>
#include <iostream>
#include <queue>

#include "event.hpp"

namespace sim {

void Scheduler::tick() {
    if (!m_events.empty()) {
        auto event = m_events.top();
        m_events.pop();
        event();
    } else {
        logger->warn("No events to process.");
    }
}

void Scheduler::add(const Event& event) { m_events.emplace(event); }

}  // namespace sim
