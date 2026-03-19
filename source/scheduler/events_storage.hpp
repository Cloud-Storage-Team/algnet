#pragma once
#include <spdlog/fmt/fmt.h>

#include <array>
#include <bitset>
#include <queue>

#include "event/event.hpp"
#include "types.hpp"

namespace sim {

template <std::size_t Size>
class EventsStorage {
public:
    void add(std::unique_ptr<Event>&& event, std::size_t index) {
        std::size_t cap = capacity();
        if (index >= cap) {
            throw std::runtime_error(
                fmt::format("Try to add event by index {} to storage with "
                            "that is greater or equal to capacity {}",
                            index, cap));
        }
        index += m_current_bucket_index;
        m_buckets[index].emplace(std::move(event));
        m_not_empty_buckets[index] = true;
        m_empty = false;
    }

    std::size_t capacity() const { return Size - m_current_bucket_index - 1; }

    bool empty() {
        correctify_state();
        return m_empty;
    }

    std::unique_ptr<Event>& top() {
        if (empty()) {
            throw std::runtime_error(
                "Try to pop first event from empty storage");
        }
        // state is correct

        return m_buckets[m_current_bucket_index].front();
    }

    void pop() {
        if (empty()) {
            throw std::runtime_error("Pop from empty events storage");
        }
        m_buckets[m_current_bucket_index].pop();
    }

    void clear() {
        for (auto& bucket : m_buckets) {
            std::queue<std::unique_ptr<Event> >().swap(bucket);
        }
        m_not_empty_buckets.reset();
        m_current_bucket_index = 0;
        m_empty = true;
    }

private:
    void correctify_state() {
        if (m_empty) {
            return;
        }
        while (m_current_bucket_index < Size &&
               m_buckets[m_current_bucket_index].empty()) {
            m_not_empty_buckets[m_current_bucket_index] = false;
            m_current_bucket_index =
                m_not_empty_buckets._Find_next(m_current_bucket_index);
        }

        if (m_current_bucket_index == Size) {
            m_current_bucket_index = 0;
            m_empty = true;
        }
    }

    std::array<std::queue<std::unique_ptr<Event> >, Size> m_buckets;
    std::bitset<Size> m_not_empty_buckets;

    bool m_empty = true;
    std::size_t m_current_bucket_index = 0;
};

}  // namespace sim