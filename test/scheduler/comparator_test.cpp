#include <gtest/gtest.h>

#include "scheduler.hpp"
#include "utils.hpp"

std::uint32_t sim::ComparatorEvent::last_time;

TEST(Compare, ExpectedProcessingOrder) {
    int number_of_events = 100;

    sim::ComparatorEvent::last_time = 0;
    sim::AddEvents<sim::ComparatorEvent>(number_of_events);

    while (sim::Scheduler::get_instance().tick()) {
    }

    sim::Scheduler::get_instance().clear();
}
