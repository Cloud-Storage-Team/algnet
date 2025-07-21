#pragma once
#include "size.hpp"
#include "time.hpp"

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase>
class Speed {
public:
    constexpr Speed(Size<TSizeBase> size, Time<TTimeBase> time)
        : m_bit_per_ns(size.get_bits() / time.get_nanoseconds()) {}

    template <IsSizeBase USizeBase, IsTimeBase UTimeBase>
    constexpr Speed(Speed<USizeBase, UTimeBase> speed)
        : m_bit_per_ns(speed.value_bit_per_ns()) {}

    constexpr long double value() const {
        return m_bit_per_ns / TSizeBase::to_bit_multiplier *
               TTimeBase::to_nanoseconds_multiplier;
    }

    constexpr long double value_bit_per_ns() const { return m_bit_per_ns; }

private:
    long double m_bit_per_ns;  // value in bit per nanosecond
};

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase>
std::ostream& operator<<(std::ostream& out, Speed<TSizeBase, TTimeBase> time) {
    return out << time.value();
}
