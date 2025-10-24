#pragma once
#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <cstddef>
#include <optional>
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
        : m_have_records(false), m_factor(a_factor), m_mean(0), m_variance(0) {
        if (m_factor < 0 || m_factor > 1) {
            LOG_ERROR(
                fmt::format("Passed incorrect factor; expected value in [0, "
                            "1], but given {}",
                            m_factor));
        }
    }

    void add_record(const T& record) {
        m_have_records = true;
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

    std::optional<T> get_last() const {
        return (m_have_records ? std::make_optional<T>(m_last) : std::nullopt);
    }

    std::optional<T> get_mean() const {
        return (m_have_records ? std::make_optional<T>(m_mean) : std::nullopt);
    }

    std::optional<double> get_variance() {
        return (m_have_records ? std::make_optional<double>(m_variance)
                               : std::nullopt);
    }

    std::optional<T> get_std() const {
        return (m_have_records ? std::make_optional<T>(std::sqrt(m_variance))
                               : std::nullopt);
    }

private:
    bool m_have_records;
    const double m_factor;
    double m_last;
    double m_mean;
    double m_variance;
};

}  // namespace utils
