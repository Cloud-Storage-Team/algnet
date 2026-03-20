#pragma once
#include <spdlog/fmt/fmt.h>

#include <array>
#include <bitset>
#include <cassert>
#include <queue>

#include "event/new_event.hpp"
#include "types.hpp"

namespace sim {

template <std::size_t Size>
class EventsStorage {
public:
    void add(NewEvent&& event, std::size_t index) {
        if (index >= Size) {
            throw std::runtime_error(
                fmt::format("Try to add event by index {} to storage with "
                            "that is greater or equal to container size {}",
                            index, Size));
        }
        m_buckets[index].emplace(std::move(event));
        m_not_empty_buckets[index] = true;
        ++m_packets_count;
    }

    bool empty() const { return m_packets_count == 0; }

    const std::queue<NewEvent>& operator[](std::size_t index) const {
        return m_buckets[index];
    }

    NewEvent& top_from(std::size_t index) {
        if (m_buckets[index].empty()) {
            throw std::runtime_error(fmt::format(
                "Try to get top from empty storage by index {}", index));
        }
        return m_buckets[index].front();
    }

    void pop_from(std::size_t index) {
        if (m_buckets[index].empty()) {
            throw std::runtime_error(
                fmt::format("Pop from empty bucket by index {}", index));
        }
        m_buckets[index].pop();
        m_packets_count--;
        if (m_buckets[index].empty()) {
            m_not_empty_buckets[index] = false;
        }
    }

    void clear() {
        for (auto& bucket : m_buckets) {
            std::queue<NewEvent>().swap(bucket);
        }
        m_not_empty_buckets.reset();
        m_packets_count = 0;
    }

    std::size_t find_next_not_empty(std::size_t index) {
        if (empty()) {
            return 0;
        }
        while (m_buckets[index].empty() && index < Size) {
            index = m_not_empty_buckets._Find_next(index);
        }
        if (index == Size) {
            throw std::runtime_error("Next not empty not found");
        }
        assert(!m_buckets[index].empty());
        return index;
    }

private:
    // void correctify_state() {
    //     if (m_packets_count == 0) {
    //         m_current_bucket_index = 0;
    //     } else {
    //         while (m_current_bucket_index < Size &&
    //                m_buckets[m_current_bucket_index].empty()) {
    //             m_not_empty_buckets[m_current_bucket_index] = false;
    //             m_current_bucket_index =
    //                 m_not_empty_buckets._Find_next(m_current_bucket_index);
    //         }
    //         assert(m_current_bucket_index < Size);
    //     }
    // }

    std::array<std::queue<NewEvent>, Size> m_buckets;
    std::bitset<Size> m_not_empty_buckets;

    std::size_t m_packets_count = 0;
};

}  // namespace sim