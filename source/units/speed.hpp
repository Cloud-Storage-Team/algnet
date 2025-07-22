#pragma once
#include "size.hpp"
#include "time.hpp"

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase>
class Speed {
private:
    using ThisSpeed = Speed<TSizeBase, TTimeBase>;

public:
    // Attention: value given in TSizeBase per TTimeBase units!
    constexpr explicit Speed(long double value)
        : m_bit_per_ns(value * TSizeBase::to_bit_multiplier /
                       TTimeBase::to_nanoseconds_multiplier) {}

    template <IsSizeBase USizeBase, IsTimeBase UTimeBase>
    constexpr Speed(Speed<USizeBase, UTimeBase> speed)
        : m_bit_per_ns(speed.value_bit_per_ns()) {}

    constexpr long double value() const {
        return m_bit_per_ns / TSizeBase::to_bit_multiplier *
               TTimeBase::to_nanoseconds_multiplier;
    }

    constexpr long double value_bit_per_ns() const { return m_bit_per_ns; }

    constexpr bool operator==(ThisSpeed speed) const {
        return equal(m_bit_per_ns, speed.value_bit_per_ns());
    }

    constexpr bool operator!=(ThisSpeed speed) const {
        return !(*this == speed);
    }

private:
    long double m_bit_per_ns;  // value in bit per nanosecond
};

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase>
std::ostream& operator<<(std::ostream& out, Speed<TSizeBase, TTimeBase> time) {
    return out << time.value();
}
