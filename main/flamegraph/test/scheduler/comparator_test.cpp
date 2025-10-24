#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

TEST_F(TestScheduler, ExpectedProcessingOrder) {
    int number_of_events = 5;

    ComparatorEvent::last_time = TimeNs(0);
    AddEvents<ComparatorEvent>(number_of_events);

    while (sim::Scheduler::get_instance().tick()) {
    }
}

}  // namespace test
