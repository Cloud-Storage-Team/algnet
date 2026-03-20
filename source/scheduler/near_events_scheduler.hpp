#pragma once
#include <deque>
#include <queue>

#include "event/new_event.hpp"
#include "events_storage.hpp"
#include "types.hpp"

namespace sim {

template <typename Func>
concept TFetchTime = std::invocable<Func, TimeNs>;

template <std::size_t Size>
class NearEventsScheduler {
public:
    void add(NewEvent&& event) {
        assert(event.time >= m_current_time);
        std::uint64_t first_bucket_index =
            m_current_bucket_index +
            (event.time - m_current_time).value_nanoseconds();

        if (first_bucket_index < Size) {
            m_storage_pair.first().add(std::move(event), first_bucket_index);
        } else {
            m_storage_pair.second().add(std::move(event),
                                        first_bucket_index - Size);
        }
    }

    void set_current_time(TimeNs new_current_time) {
        if (!empty()) {
            throw std::runtime_error(
                "Call set_current_time on not empty NearEventsStorage");
        }
        m_current_bucket_index = 0;
        m_current_time = new_current_time;
    }

    // calls first event & returns its time
    // throws error in case of no events
    template <TFetchTime Func>
    void tick(Func&& fetch_time) {
        if (empty()) {
            throw std::runtime_error("Tick when storage is empty");
        }
        correctify_state();
        EventsStorage<Size>& first_storage = m_storage_pair.first();

        NewEvent event =
            std::move(first_storage.top_from(m_current_bucket_index));
        first_storage.pop_from(m_current_bucket_index);

        m_current_time = event.time;
        std::invoke(std::forward<Func>(fetch_time), m_current_time);

        event.call();
    }

    bool empty() const { return m_storage_pair.empty(); }

    void clear() {
        m_storage_pair.clear();
        m_current_bucket_index = 0;
        m_current_time = TimeNs(0);
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

        void swap_storages() { m_swaped ^= 1; }

        void clear() {
            m_swaped = false;
            m_first.clear();
            m_second.clear();
        }

        bool empty() const { return m_first.empty() && m_second.empty(); }

    private:
        EventsStorage<Size> m_first;
        EventsStorage<Size> m_second;
        bool m_swaped = false;
    };

    void correctify_state() {
        if (m_storage_pair.first().empty()) {
            m_storage_pair.swap_storages();
            m_current_bucket_index = 0;
        }
        correctify_bucket_index();
    }

    void correctify_bucket_index() {
        m_current_bucket_index =
            m_storage_pair.first().find_next_not_empty(m_current_bucket_index);
        assert(empty() ||
               !m_storage_pair.first()[m_current_bucket_index].empty());
    }

    StoragePair m_storage_pair;
    std::size_t m_current_bucket_index = 0;
    TimeNs m_current_time = TimeNs(0);
};

}  // namespace sim
