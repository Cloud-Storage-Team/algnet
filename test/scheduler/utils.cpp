#include "utils.hpp"

namespace test {

int CountingEvent::cnt;
std::uint32_t ComparatorEvent::last_time;

void EmptyEvent::operator()() {}

void CountingEvent::operator()() { cnt++; }

void ComparatorEvent::operator()() {
    EXPECT_GE(time, last_time);
    last_time = time;
}

template <typename T>
void AddEvents(int number, std::shared_ptr<std::uint32_t> event_time) {
    static_assert(std::is_base_of<sim::Event, T>::value,
                  "T must inherit from Event");

    srand(static_cast<unsigned int>(time(0)));
    std::uint32_t min_time = 1;
    std::uint32_t max_time = static_cast<std::uint32_t>(1e9);

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

template void AddEvents<sim::Stop>(int, std::shared_ptr<std::uint32_t>);
template void AddEvents<EmptyEvent>(int, std::shared_ptr<std::uint32_t>);
template void AddEvents<CountingEvent>(int, std::shared_ptr<std::uint32_t>);
template void AddEvents<ComparatorEvent>(int, std::shared_ptr<std::uint32_t>);

}  // namespace test
