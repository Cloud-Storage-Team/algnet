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

template <typename T>
void AddEvents(int number, std::shared_ptr<Time> event_time) {
    static_assert(std::is_base_of<sim::Event, T>::value,
                  "T must inherit from Event");

    srand(static_cast<unsigned int>(time(0)));
    Time min_time = 1;
    Time max_time = static_cast<Time>(1e9);

    while ((number--) > 0) {
        if (event_time == nullptr) {
            sim::Scheduler::get_instance().add<T>(
                rand() % (max_time - min_time + 1) + min_time);
        } else {
            sim::Scheduler::get_instance().add<T>(++(*event_time.get()));
        }
    }
}

template void AddEvents<sim::Stop>(int, std::shared_ptr<Time>);
template void AddEvents<EmptyEvent>(int, std::shared_ptr<Time>);
template void AddEvents<CountingEvent>(int, std::shared_ptr<Time>);
template void AddEvents<ComparatorEvent>(int, std::shared_ptr<Time>);

}  // namespace test
