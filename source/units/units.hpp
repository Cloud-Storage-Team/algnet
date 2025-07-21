#pragma once

#include "size.hpp"
#include "speed.hpp"
#include "time.hpp"

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase>
constexpr Speed<TSizeBase, TTimeBase> operator/(Size<TSizeBase> size,
                                                Time<TTimeBase> time) {
    return Speed(size, time);
}