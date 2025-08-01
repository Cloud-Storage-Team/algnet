#pragma once
#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <cstddef>
#include <type_traits>

#include "logger/logger.hpp"

namespace utils {

template <typename T, typename U>
concept ExplicitlyConvertable = requires(T t) { static_cast<U>(t); };

template <typename T>
requires ExplicitlyConvertable<T, long double>
class Statistics {
public:
    explicit Statistics(long double a_factor)
        : m_factor(a_factor), m_mean(0), m_variance(0) {
        if (m_factor < 0 || m_factor > 1) {
            LOG_ERROR(
                fmt::format("Passed incorrect factor; expected value in [0, "
                            "1], but given {}",
                            m_factor));
        }
    }

    void add_record(const T& record) {
        long double value = static_cast<long double>(record);
        if (m_mean == 0 && m_variance == 0) {
            // first record
            m_mean = value;
        } else {
            long double delta = value - m_mean;
            m_mean = m_mean * m_factor + value * (1 - m_factor);
            m_variance =
                m_variance * m_factor + (delta * delta) * (1 - m_factor);
        }
    }

    T get_mean() const { return T(m_mean); }

    long double get_variance() { return m_variance; }

    T get_std() const { return T(std::sqrt(m_variance)); }

private:
    const long double m_factor;
    long double m_mean;
    long double m_variance;
};

}  // namespace utils
