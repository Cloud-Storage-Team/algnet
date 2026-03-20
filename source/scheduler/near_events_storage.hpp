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
    void add(NewEvent&& event) {
        assert(event.time >= m_first_storage_first_event_time +
                                 TimeNs(m_first_storage_bucket_index));
        std::uint64_t first_bucket_index =
            (event.time - m_first_storage_first_event_time).value_nanoseconds();

        if (first_bucket_index < Size) {
            m_storage_pair.first().add(std::move(event), first_bucket_index);
        } else {
            m_storage_pair.second().add(std::move(event),
                                        first_bucket_index - Size);
        }
    }

    NewEvent& top() {
        if (empty()) {
            throw std::runtime_error(
                "Try to take event from empty near events storage");
        }
        auto& res =
            m_storage_pair.first().top_from(m_first_storage_bucket_index);
        return res;
    }

    void pop() {
        m_storage_pair.first().pop_from(m_first_storage_bucket_index);
        correctify_state();
    }

    bool empty() const { return m_storage_pair.first().empty(); }

    void clear() {
        m_storage_pair.clear();
        m_first_storage_bucket_index = 0;
    }

private:
    class StoragePair {
    public:
        EventsStorage<Size>& first() { return (m_swaped ? m_second : m_first); }
        const EventsStorage<Size>& first() const {
            return (m_swaped ? m_second : m_first);
        }

        EventsStorage<Size>& second() { return m_swaped ? m_first : m_second; }
        const EventsStorage<Size>& second() const {
            return m_swaped ? m_first : m_second;
        }

        void swap_storages() {
            m_swaped ^= 1;
            std::cout << "storages swaped" << std::endl;
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

    void correctify_state() {
        if (m_storage_pair.first().empty()) {
            m_storage_pair.swap_storages();
            m_first_storage_first_event_time += TimeNs(Size);
            m_first_storage_bucket_index = 0;
        }
        correctify_bucket_index();
    }

    void correctify_bucket_index() {
        m_first_storage_bucket_index =
            m_storage_pair.first().find_next_not_empty(
                m_first_storage_bucket_index);
        assert(empty() ||
               !m_storage_pair.first()[m_first_storage_bucket_index].empty());
    }

    StoragePair m_storage_pair;
    std::size_t m_first_storage_bucket_index = 0;
    TimeNs m_first_storage_first_event_time = TimeNs(0);
};

}  // namespace sim