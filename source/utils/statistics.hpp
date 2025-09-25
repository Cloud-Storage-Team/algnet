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
requires ExplicitlyConvertable<T, double>
class Statistics {
public:
    explicit Statistics(double a_factor = 0.8)
        : m_factor(a_factor), m_mean(0), m_variance(0) {
        if (m_factor < 0 || m_factor > 1) {
            LOG_ERROR(
                fmt::format("Passed incorrect factor; expected value in [0, "
                            "1], but given {}",
                            m_factor));
        }
    }

    void add_record(const T& record) {
        double value = static_cast<double>(record);
        m_last = value;
        if (m_mean == 0 && m_variance == 0) {
            // first record
            m_mean = value;
        } else {
            double delta = value - m_mean;
            m_mean = m_mean * m_factor + value * (1 - m_factor);
            m_variance =
                m_variance * m_factor + (delta * delta) * (1 - m_factor);
        }
    }

    T get_last() const { return T(m_last); }

    T get_mean() const { return T(m_mean); }

    double get_variance() { return m_variance; }

    T get_std() const { return T(std::sqrt(m_variance)); }

private:
    const double m_factor;
    double m_last;
    double m_mean;
    double m_variance;
};

}  // namespace utils
