#pragma once

#include <iterator>
#include <stdexcept>

namespace sim {

template <std::forward_iterator Iter>
class LoopIterator {
public:
    using value_t = typename std::iterator_traits<Iter>::value_type;
    using value_ref_t = typename std::iterator_traits<Iter>::reference;

    LoopIterator() = default;

    LoopIterator(Iter a_begin, Iter a_end)
        : m_curr(a_begin), m_begin(a_begin), m_end(a_end) {
        if (a_begin == a_end) {
            throw std::runtime_error("LoopIterator range cannot be empty");
        }
    }

    value_ref_t operator*() const { return *m_curr; }

    LoopIterator& operator++() {
        if (++m_curr == m_end) {
            m_curr = m_begin;
        }
        return *this;
    }

    LoopIterator operator++(int) {
        auto iter = *this;
        ++(*this);
        return iter;
    }

private:
    Iter m_curr;
    Iter m_begin;
    Iter m_end;
};

}  // namespace sim
