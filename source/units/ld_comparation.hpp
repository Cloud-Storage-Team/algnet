#pragma once
#include <cmath>

constexpr bool equal(long double value_1, long double value_2) {
    constexpr long double EPS = 1e-6;
    return abs(value_1 - value_2) < EPS;
}
