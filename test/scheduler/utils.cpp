#include "utils.hpp"

namespace test {

int CountingEvent::cnt;
Time ComparatorEvent::last_time;

EmptyEvent::EmptyEvent(std::uint32_t a_time) : Event(a_time) {};
void EmptyEvent::operator()() {}

CountingEvent::CountingEvent(std::uint32_t a_time) : Event(a_time) {};
void CountingEvent::operator()() { cnt++; }

ComparatorEvent::ComparatorEvent(std::uint32_t a_time) : Event(a_time) {};
void ComparatorEvent::operator()() {
    EXPECT_GE(m_time, last_time);
    last_time = m_time;
}

}  // namespace test
