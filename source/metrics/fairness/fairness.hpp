#pragma once
#include <spdlog/fmt/fmt.h>

#include "utils/explicitly_convertable.hpp"
#include "utils/id_table.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

// Jain's fairness index https://reimbar.org/posts/jain-fairness/
template <utils::ExplicitlyConvertable<double> T>
class JainsFairnessIndex {
public:
    JainsFairnessIndex() : m_sum(0), m_squares_sum(0) {}

    explicit JainsFairnessIndex(std::unordered_map<Id, T> a_values)
        : m_values(), m_sum(0.0), m_squares_sum(0.0) {
        for (const auto& [id, value] : a_values) {
            double casted_value = static_cast<double>(value);
            m_values.emplace(id, casted_value);
            m_sum += casted_value;
            m_squares_sum += casted_value * casted_value;
        }
    }

    template <typename U>
    explicit JainsFairnessIndex(const utils::IdTable<U>& id_table)
        : JainsFairnessIndex() {
        for (const auto& [id, ptr] : id_table) {
            m_values.emplace(id, 0);
        }
    }

    // updates value for id & returns new fairness
    double update(const Id& id, T val) {
        auto it = m_values.find(id);
        if (it == m_values.end()) {
            throw std::runtime_error(
                fmt::format("Could not update fairness for id {}: no sush id "
                            "in valued table (probably you forgot to add it to "
                            "initial values map in costructor)",
                            id));
        }
        double& table_value = it->second;
        double casted_value = static_cast<double>(val);

        // update sum
        m_sum -= table_value;
        m_sum += casted_value;

        // update square sum
        m_squares_sum -= table_value * table_value;
        m_squares_sum += casted_value * casted_value;

        // update value in table
        table_value = casted_value;
        return value();
    }

    double value() const {
        return m_sum * m_sum / (m_values.size() * m_squares_sum);
    }

private:
    // Optimization: to recalculate metric faster, store values (in m_values),
    // its sum & sum of squares
    std::unordered_map<Id, double> m_values;

    double m_sum;
    double m_squares_sum;
};

}  // namespace sim