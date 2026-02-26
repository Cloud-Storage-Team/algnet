#pragma once
#include <spdlog/fmt/fmt.h>

#include "utils/explicitly_convertable.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

// Jain's fairness index https://reimbar.org/posts/jain-fairness/
template <utils::ExplicitlyConvertable<double> T>
class JainsFairnessIndex {
public:
    explicit JainsFairnessIndex(std::unordered_map<Id, T> a_values = {})
        : m_values(), m_sum(0.0), m_squiares_sum(0.0) {
        for (const auto& [id, value] : a_values) {
            double casted_value = static_cast<double>(value);
            m_values.emplace(id, casted_value);
            m_sum += casted_value;
            m_squiares_sum += casted_value * casted_value;
        }
    }

    // updates value for id & returs new fairness
    T update(const Id& id, T val) {
        auto it = m_values.find(id);
        if (it == m_values.end()) {
            it = m_values.emplace(id, 0.0).first;
            LOG_INFO(fmt::format("Added zero value for new id {}", id));
        }
        double& table_value = it->second;
        double casted_value = static_cast<double>(val);

        // update sum
        m_sum -= table_value;
        m_sum += casted_value;

        // updte square sum
        m_squiares_sum -= table_value * table_value;
        m_squiares_sum += casted_value * casted_value;

        // update value in table
        table_value = casted_value;
        return value();
    }

    T value() const {
        return T(m_sum * m_sum / (m_values.size() * m_squiares_sum));
    }

private:
    // Optimization: to recalculate metric faster, store values (in m_values),
    // its sum & sum of squares
    std::unordered_map<Id, double> m_values;

    double m_sum;
    double m_squiares_sum;
};

}  // namespace sim