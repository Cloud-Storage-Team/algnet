#include "ecn.hpp"

#include <spdlog/fmt/fmt.h>

#include "logger/logger.hpp"

// Throws std::invalid argument if value < min_value ot value > max_value
static void check_value_limits(float value, float min_value, float max_value,
                               std::string value_name = "") {
    std::string error_prefix = fmt::format(
        "{} = {}", (value_name == "" ? "Given value" : value_name), value);
    if (value < min_value) {
        throw std::invalid_argument(fmt::format(
            "{} is less minimal threshold {}", error_prefix, min_value));
    }
    if (value > max_value) {
        throw std::invalid_argument(fmt::format(
            "{} is more maximal threshold {}", error_prefix, max_value));
    }
}

static void soft_check_value_limits(float value, float min_value,
                                    float max_value,
                                    std::string value_name = "") {
    try {
        check_value_limits(value, min_value, max_value, value_name);
    } catch (const std::exception& e) {
        LOG_ERROR(e.what());
    }
}

namespace sim {
ECN::ECN(float a_min, float a_max, float a_probability)
    : m_min(a_min),
      m_max(a_max),
      m_probability(a_probability),
      m_distribution(0.0, 1.0),
      m_generator(std::random_device()()) {
    if (m_min > m_max) {
        throw std::invalid_argument("Min threshold more than max threshold");
    }
    // Use soft check due to class correct works with violated restrictions
    soft_check_value_limits(m_min, 0.0, 1.0, "ECN min");
    soft_check_value_limits(m_max, 0.0, 1.0, "ECN max");
    soft_check_value_limits(m_probability, 0.0, 1.0, "ECN probability");
}

bool ECN::get_congestion_mark(float queue_filling) const {
    if (queue_filling < m_min) {
        return false;
    }
    if (queue_filling > m_max) {
        return true;
    }
    float interpolated_probability =
        m_probability * (queue_filling - m_min) / (m_max - m_min);
    return m_distribution(m_generator) < interpolated_probability;
}

std::ostream& operator<<(std::ostream& out, const ECN& ecn) {
    out << "min: " << ecn.m_min << "\n";
    out << "max : " << ecn.m_max << "\n";
    out << "; probability : " << ecn.m_probability << '\n';
    return out;
}

}  // namespace sim