#pragma once
#include <deque>
#include <queue>

#include "event/event.hpp"
#include "types.hpp"

namespace sim {

class NearEventsStorage {
public:
    explicit NearEventsStorage(std::size_t a_max_buckets_count)
        : m_current_bucket(0), m_max_buckets_count(a_max_buckets_count) {
        m_first_event_buckets_storage.emplace_back(
            std::queue<std::unique_ptr<Event> >());
        m_second_event_buckets_storage.emplace_back(
            std::queue<std::unique_ptr<Event> >());
    }

    void add(std::unique_ptr<Event> event, TimeNs current_time);
    std::unique_ptr<Event> pop_first();
    bool empty();

private:
    // forwards m_current_bucket while it points to empty bucket
    void correctify_state();

    std::deque<std::queue<std::unique_ptr<Event> > >
        m_first_event_buckets_storage;
    std::deque<std::queue<std::unique_ptr<Event> > >
        m_second_event_buckets_storage;

    std::size_t m_current_bucket;
    std::size_t m_max_buckets_count;

    bool m_first_bucket_empty = true;
    bool m_second_bucket_empty = true;
};

}  // namespace sim