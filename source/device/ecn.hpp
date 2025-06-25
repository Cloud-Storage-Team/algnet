#pragma once
#include <iostream>
#include <random>

#include "types.hpp"
namespace sim {

class ECN {
public:
    ECN(Size a_min, Size a_max, float a_probability);
    // returns true if congestion detected with given queue size, false
    // otherwise
    bool get_congestion_mark(Size queue_size) const;

    friend std::ostream& operator<<(std::ostream& out, const ECN& ecn);

private:
    // More information about these fields:
    // https://man7.org/linux/man-pages/man8/tc-red.8.html
    Size m_min;
    Size m_max;
    float m_probability;

    mutable std::uniform_real_distribution<> m_distribution;
    mutable std::mt19937 m_generator;
};

}  // namespace sim