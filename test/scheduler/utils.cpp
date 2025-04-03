#include "utils.hpp"

namespace test {

int CountingEvent::cnt;
Time ComparatorEvent::last_time;

void EmptyEvent::operator()() {}

void CountingEvent::operator()() { cnt++; }

void ComparatorEvent::operator()() {
    EXPECT_GE(time, last_time);
    last_time = time;
}

template <typename T>
void AddEvents(int number, std::shared_ptr<Time> event_time) {
    static_assert(std::is_base_of<sim::Event, T>::value,
                  "T must inherit from Event");

    srand(static_cast<unsigned int>(time(0)));
    Time min_time = 1;
    Time max_time = static_cast<Time>(1e9);

    while ((number--) > 0) {
        std::unique_ptr<sim::Event> event_ptr = std::make_unique<T>(T());

        if (event_time == nullptr) {
            event_ptr->time = rand() % (max_time - min_time + 1) + min_time;
        } else {
            event_ptr->time = ++(*event_time.get());
        }

        sim::Scheduler::get_instance().add(std::move(event_ptr));
    }
}

template void AddEvents<sim::Stop>(int, std::shared_ptr<Time>);
template void AddEvents<EmptyEvent>(int, std::shared_ptr<Time>);
template void AddEvents<CountingEvent>(int, std::shared_ptr<Time>);
template void AddEvents<ComparatorEvent>(int, std::shared_ptr<Time>);

}  // namespace test
