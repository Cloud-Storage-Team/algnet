#pragma once

#include "size.hpp"
#include "speed.hpp"
#include "time.hpp"

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase>
constexpr Speed<TSizeBase, TTimeBase> operator/(Size<TSizeBase> size,
                                                Time<TTimeBase> time) {
    return Speed<Bit, Nanosecond>(size.value_bits() / time.value_nanoseconds());
}

template <IsSizeBase TSpeedSizeBase, IsTimeBase TSpeedTimeBase,
          IsTimeBase TTimeBase>
constexpr Size<Bit> operator*(Speed<TSpeedSizeBase, TSpeedTimeBase> speed,
                              Time<TTimeBase> time) {
    return Size<Bit>(speed.m_bit_per_ns * time.value_nanoseconds());
}

template <IsSizeBase TSizeBase, IsSizeBase TSpeedSizeBase,
          IsTimeBase TSpeedTimeBase>
constexpr Time<Nanosecond> operator/(
    Size<TSizeBase> size, Speed<TSpeedSizeBase, TSpeedTimeBase> speed) {
    return Time<Nanosecond>(size.value_bits() / speed.value_bit_per_ns());
}
