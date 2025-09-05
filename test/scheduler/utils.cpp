#include "utils.hpp"

namespace test {

int CountingEvent::cnt;
TimeNs ComparatorEvent::last_time(0);

EmptyEvent::EmptyEvent(TimeNs a_time) : Event(a_time){};
void EmptyEvent::operator()() {}

CountingEvent::CountingEvent(TimeNs a_time) : Event(a_time){};
void CountingEvent::operator()() { cnt++; }

ComparatorEvent::ComparatorEvent(TimeNs a_time) : Event(a_time){};
void ComparatorEvent::operator()() {
    EXPECT_GT(m_time, last_time);
    last_time = m_time;
}

}  // namespace test
