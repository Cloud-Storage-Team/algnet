#pragma once
#include <iostream>
#include <random>

#include "types.hpp"
namespace sim {

class ECN {
public:
    ECN(float a_min, float a_max, float a_probability);
    // returns true if congestion detected with given queue size, false
    // otherwise
    bool get_congestion_mark(float queue_filling) const;

    friend std::ostream& operator<<(std::ostream& out, const ECN& ecn);

private:
    // More information about these fields:
    // https://man7.org/linux/man-pages/man8/tc-red.8.html
    // differences from man: min and max are not absolute buffer size values,
    // they represents same thresholds in percents
    float m_min;
    float m_max;
    float m_probability;

    mutable std::uniform_real_distribution<> m_distribution;
    mutable std::mt19937 m_generator;
};

}  // namespace sim