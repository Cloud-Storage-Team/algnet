#include "utils.hpp"

#include <type_traits>

#include "event.hpp"

namespace test {

int CountingEvent::cnt;
Time ComparatorEvent::last_time;

EmptyEvent::EmptyEvent(std::uint32_t a_time) : m_time(a_time) {};
void EmptyEvent::operator()() {}
Time EmptyEvent::get_time() const { return m_time; }

CountingEvent::CountingEvent(std::uint32_t a_time) : m_time(a_time) {};
void CountingEvent::operator()() { cnt++; }
Time CountingEvent::get_time() const { return m_time; }

ComparatorEvent::ComparatorEvent(std::uint32_t a_time) : m_time(a_time) {};
void ComparatorEvent::operator()() {
    EXPECT_GE(m_time, last_time);
    last_time = m_time;
}
Time ComparatorEvent::get_time() const { return m_time; }

StopEvent::StopEvent(std::uint32_t a_time) : m_time(a_time) {}
void StopEvent::operator()() { Scheduler::get_instance().clear(); }
Time StopEvent::get_time() const { return m_time; }

TestEvent::TestEvent(const EmptyEvent& e) : event(e) {}
TestEvent::TestEvent(const CountingEvent& e) : event(e) {}
TestEvent::TestEvent(const ComparatorEvent& e) : event(e) {}
TestEvent::TestEvent(const StopEvent& e) : event(e) {}

void TestEvent::operator()() {
    std::visit([&](auto real_event) { real_event(); }, event);
}
bool TestEvent::operator>(const TestEvent& other) const {
    return get_time() > other.get_time();
}
bool TestEvent::operator<(const TestEvent& other) const {
    return get_time() < other.get_time();
}
Time TestEvent::get_time() const {
    return std::visit([&](auto real_event) { return real_event.get_time(); },
                      event);
}

template <typename T>
void AddEvents(int number, std::shared_ptr<Time> event_time) {
    static_assert(
        std::is_constructible_v<TestEvent, T>,
        "T must be one ot the event enumerated in BaseEvent's std::variant");

    srand(static_cast<unsigned int>(time(0)));
    Time min_time = 1;
    Time max_time = static_cast<Time>(1e9);

    while ((number--) > 0) {
        if (event_time == nullptr) {
            Scheduler::get_instance().add(
                TestEvent(T(rand() % (max_time - min_time + 1) + min_time)));
        } else {
            Scheduler::get_instance().add(TestEvent(T(++(*event_time.get()))));
        }
    }
}

template void AddEvents<StopEvent>(int, std::shared_ptr<Time>);
template void AddEvents<EmptyEvent>(int, std::shared_ptr<Time>);
template void AddEvents<CountingEvent>(int, std::shared_ptr<Time>);
template void AddEvents<ComparatorEvent>(int, std::shared_ptr<Time>);

}  // namespace test
