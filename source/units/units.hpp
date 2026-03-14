#pragma once

#include <cmath>

#include "size.hpp"
#include "size_fmt.hpp"
#include "speed.hpp"
#include "speed_fmt.hpp"
#include "time.hpp"
#include "time_fmt.hpp"

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase>
constexpr Speed<TSizeBase, TTimeBase> operator/(Size<TSizeBase> size,
                                                Time<TTimeBase> time) {
    return Speed<Bit, Nanosecond>(
        size.value_bits() / static_cast<double>(time.value_nanoseconds()));
}

template <IsSizeBase TSpeedSizeBase, IsTimeBase TSpeedTimeBase,
          IsTimeBase TTimeBase>
constexpr Size<Bit> operator*(Speed<TSpeedSizeBase, TSpeedTimeBase> speed,
                              Time<TTimeBase> time) {
    return Size<Bit>(speed.m_value_bit_per_ns * time.value_nanoseconds());
}

template <IsSizeBase TSizeBase, IsSizeBase TSpeedSizeBase,
          IsTimeBase TSpeedTimeBase>
constexpr Time<Nanosecond> operator/(
    Size<TSizeBase> size, Speed<TSpeedSizeBase, TSpeedTimeBase> speed) {
    return Time<Nanosecond>(
        std::round(size.value_bits() / speed.value_bit_per_ns()));
}
