#pragma once
#include <cmath>

constexpr bool equal(double value_1, double value_2) {
    constexpr double EPS = 1e-6;
    return std::abs(value_1 - value_2) < EPS;
}
