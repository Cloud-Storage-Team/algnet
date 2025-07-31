#pragma once
#include <cstddef>

namespace utils {

class Statistics {
public:
    Statistics(long double a_factor);

    void add_record(long double record);

    long double get_mean() const;
    long double get_variance() const;

private:
    const long double m_factor;
    long double m_mean;
    long double m_variance;
};

}  // namespace utils