#pragma once
#include <deque>
#include <queue>

#include "event/new_event.hpp"
#include "events_storage.hpp"
#include "types.hpp"

namespace sim {

template <std::size_t Size>
class NearEventsStorage {
public:
    void add(NewEvent&& event, TimeNs current_time) {
        std::uint32_t relative_event_time =
            (event.time - current_time).value_nanoseconds();

        EventsStorage<Size>& first_storage = m_storage_pair.first();
        std::size_t first_bucket_capacity = first_storage.capacity();
        if (relative_event_time < first_bucket_capacity) {
            first_storage.add(std::move(event), relative_event_time);
        } else {
            m_storage_pair.second().add(
                std::move(event), relative_event_time - first_bucket_capacity);
        }
    }

    NewEvent& top() {
        if (empty()) {
            throw std::runtime_error(
                "Try to take event from empty near events storage");
        }
        // state is correct due to empty() call inside it
        return m_storage_pair.first().top();
    }

    void pop() {
        if (empty()) {
            throw std::runtime_error(
                "Try to pop event from empty near events storage");
        }
        m_storage_pair.first().pop();
    }

    bool empty() {
        correctify_state();
        return m_storage_pair.first().empty();
    }

    void clear() { m_storage_pair.clear(); }

private:
    class StoragePair {
    public:
        EventsStorage<Size>& first() { return (m_swaped ? m_second : m_first); }
        EventsStorage<Size>& second() { return m_swaped ? m_first : m_second; }

        void correctify_state() {
            if (first().empty()) {
                // first storage is empty -> swap storages
                m_swaped ^= 1;
            }
        }

        void clear() {
            m_swaped = false;
            m_first.clear();
            m_second.clear();
        }

    private:
        EventsStorage<Size> m_first;
        EventsStorage<Size> m_second;
        bool m_swaped = false;
    };

    void correctify_state() { m_storage_pair.correctify_state(); }

    StoragePair m_storage_pair;
};

}  // namespace sim