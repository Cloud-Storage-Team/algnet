#include "ecn.hpp"

namespace sim {
ECN::ECN(Size a_min, Size a_max, float a_probability)
    : m_min(a_min),
      m_max(a_max),
      m_probability(a_probability),
      m_distribution(0.0, 1.0),
      m_generator(std::random_device()()) {
    if (m_probability > 1.0) {
        throw std::invalid_argument("Given probability more than 1.0");
    }
    if (m_probability < 0.0) {
        throw std::invalid_argument("Given probability more less zero");
    }
}

bool ECN::get_congestion_mark(Size queue_size) const {
    if (queue_size < m_min) {
        return false;
    }
    if (queue_size > m_max) {
        return true;
    }
    float interpolated_probability =
        m_probability * (queue_size - m_min) / (m_max - m_min);
    return m_distribution(m_generator) < interpolated_probability;
}

std::ostream& operator<<(std::ostream& out, const ECN& ecn) {
    out << "min: " << ecn.m_min << "\n";
    out << "max : " << ecn.m_max << "\n";
    out << "; probability : " << ecn.m_probability << '\n';
    return out;
}

}  // namespace sim