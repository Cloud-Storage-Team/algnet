#include "near_events_storage.hpp"

namespace sim {

void NearEventsStorage::add(std::unique_ptr<Event> event, TimeNs current_time) {
    std::uint32_t relative_event_time =
        (event->get_time() - current_time).value_nanoseconds();

    auto add_to_storage =
        [](std::deque<std::queue<std::unique_ptr<Event> > >& storage,
           std::size_t index, std::unique_ptr<Event> event) {
            if (index >= storage.size()) {
                storage.resize(index + 1);
            }
            storage[index].emplace(std::move(event));
        };

    std::uint32_t first_storage_bucket_index =
        m_current_bucket + relative_event_time;
    if (first_storage_bucket_index < m_max_buckets_count) {
        // its enough place to put event to first bucket; use it
        add_to_storage(m_first_event_buckets_storage,
                       first_storage_bucket_index, std::move(event));
        m_first_bucket_empty = false;
    } else {
        // event could not be stores in first bucket; put it to second one
        add_to_storage(m_second_event_buckets_storage,
                       first_storage_bucket_index - m_max_buckets_count,
                       std::move(event));
        m_second_bucket_empty = false;
    }
}

std::unique_ptr<Event> NearEventsStorage::pop_first() {
    correctify_state();
    // state is correct
    if (empty()) {
        throw std::runtime_error("call from on empty storage");
    }
    std::unique_ptr<Event> event =
        std::move(m_first_event_buckets_storage[m_current_bucket].front());
    // be careful: after next line state should be correctified (current bucket
    // might become empty or all m_first_storage)
    m_first_event_buckets_storage[m_current_bucket].pop();
    return event;
}

bool NearEventsStorage::empty() {
    correctify_state();
    return m_first_bucket_empty;
}

void NearEventsStorage::correctify_state() {
    if (!m_first_bucket_empty) {
        while (m_current_bucket < m_first_event_buckets_storage.size() &&
               m_first_event_buckets_storage[m_current_bucket].empty()) {
            m_current_bucket++;
        }
        if (m_current_bucket != m_first_event_buckets_storage.size()) {
            // m_first_event_buckets_storage[m_current_bucket] is not empty ->
            // state is correct
            return;
        }
    }

    // all events in first storages are done => first bucket is <<empty>>
    m_first_bucket_empty = true;
    m_current_bucket = 0;

    if (m_second_bucket_empty) {
        // second bucket is empty => all buckets are empty
        return;
    }

    while (m_current_bucket < m_first_event_buckets_storage.size() &&
           m_second_event_buckets_storage[m_current_bucket].empty()) {
        m_current_bucket++;
    }

    // swap storages
    m_first_event_buckets_storage.swap(m_second_event_buckets_storage);
    std::swap(m_first_bucket_empty, m_second_bucket_empty);
}

}  // namespace sim