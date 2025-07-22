#pragma once

#include "size.hpp"
#include "speed.hpp"
#include "time.hpp"

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase>
constexpr Speed<TSizeBase, TTimeBase> operator/(Size<TSizeBase> size,
                                                Time<TTimeBase> time) {
    return Speed(size, time);
}

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase>
constexpr Size<Bit> operator*(Speed<TSizeBase, TTimeBase> speed,
                              Time<TTimeBase> time) {
    return Size<Bit>(speed.m_bit_per_ns * time.get_nanoseconds());
}

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase>
constexpr Time<TTimeBase> operator/(Size<TSizeBase> size,
                                    Speed<TSizeBase, TTimeBase> speed) {
    return Time<Nanosecond>(size.get_bits() / speed.value_bit_per_ns());
}
