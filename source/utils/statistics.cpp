#include "statistics.hpp"

#include <spdlog/fmt/fmt.h>

#include <algorithm>

#include "logger/logger.hpp"

namespace utils {

Statistics::Statistics(long double a_factor)
    : m_factor(a_factor), m_mean(0), m_variance(0) {
    if (m_factor < 0 || m_factor > 1) {
        LOG_ERROR(fmt::format(
            "Passed incorrect factor; expected value in [0, 1], but given {}",
            m_factor));
    }
};

void Statistics::add_record(long double record) {
    if (m_mean == 0 && m_variance == 0) {
        // first record
        m_mean = record;
    } else {
        long double delta = record - m_mean;
        m_mean = m_mean * m_factor + record * (1 - m_factor);
        long double old_variance = m_variance;
        m_variance = m_variance * m_factor + (delta * delta) * (1 - m_factor);
        if (m_variance > old_variance * 2) {
            LOG_WARN(
                fmt::format("Big variance expention on record {}", record));
        }
    }
}

long double Statistics::get_mean() const { return m_mean; }

long double Statistics::get_variance() const { return m_variance; }

long double Statistics::get_std() const { return std::sqrt(m_variance); }

}  // namespace utils