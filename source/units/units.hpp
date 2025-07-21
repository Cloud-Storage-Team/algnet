#pragma once

#include "size.hpp"
#include "time.hpp"

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase>
constexpr double operator/(Size<TSizeBase> size, Time<TTimeBase> time) {
    return size.get_bits() / (double)time.get_nanoseconds();
}