#include <gtest/gtest.h>

#include "scheduler.hpp"
#include "utils.hpp"

int sim::CountingEvent::cnt;

TEST(Add, ExpectedAmountOfElements) {
    int number_of_events = 100;

    sim::CountingEvent::cnt = 0;
    sim::AddEvents<sim::CountingEvent>(number_of_events);

    while (sim::Scheduler::get_instance().tick()) {
    }

    EXPECT_EQ(sim::CountingEvent::cnt, number_of_events);

    sim::Scheduler::get_instance().clear();
}
