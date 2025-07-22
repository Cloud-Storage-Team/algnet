#pragma once
#include "size.hpp"
#include "speed.hpp"

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase>
class Speed {
private:
    using ThisSpeed = Speed<TSizeBase, TTimeBase>;

public:
    // Attention: value given in TSizeBase per TTimeBase units!
    constexpr explicit Speed(long double value)
        : m_value_bit_per_ns(value * TSizeBase::to_bit_multiplier /
                             TTimeBase::to_nanoseconds_multiplier) {}

    template <IsSizeBase USizeBase, IsTimeBase UTimeBase>
    constexpr Speed(Speed<USizeBase, UTimeBase> speed)
        : m_value_bit_per_ns(speed.value_bit_per_ns()) {}

    constexpr long double value() const {
        return m_value_bit_per_ns / TSizeBase::to_bit_multiplier *
               TTimeBase::to_nanoseconds_multiplier;
    }

    constexpr long double value_bit_per_ns() const {
        return m_value_bit_per_ns;
    }

    constexpr ThisSpeed operator+(ThisSpeed speed) const {
        return Speed<Bit, Nanosecond>(m_value_bit_per_ns +
                                      speed.m_value_bit_per_ns);
    }

    constexpr ThisSpeed operator-(ThisSpeed speed) const {
        return Speed<Bit, Nanosecond>(m_value_bit_per_ns -
                                      speed.m_value_bit_per_ns);
    }

    constexpr ThisSpeed operator*(long double mult) const {
        return Speed<Bit, Nanosecond>(m_value_bit_per_ns * mult);
    }

    constexpr long double operator/(ThisSpeed speed) const {
        return m_value_bit_per_ns / speed.value_bit_per_ns();
    }

    constexpr void operator+=(ThisSpeed speed) {
        m_value_bit_per_ns += speed.m_value_bit_per_ns;
    }
    constexpr void operator-=(ThisSpeed speed) {
        m_value_bit_per_ns -= speed.m_value_bit_per_ns;
    }

    constexpr void operator*=(long double mult) { m_value_bit_per_ns *= mult; }

    constexpr void operator/=(long double mult) { m_value_bit_per_ns /= mult; }

    bool constexpr operator<(ThisSpeed speed) const {
        return m_value_bit_per_ns < speed.m_value_bit_per_ns;
    }

    bool constexpr operator>(ThisSpeed speed) const {
        return m_value_bit_per_ns > speed.m_value_bit_per_ns;
    }

    constexpr bool operator==(ThisSpeed speed) const {
        return equal(m_value_bit_per_ns, speed.value_bit_per_ns());
    }

    constexpr bool operator!=(ThisSpeed speed) const {
        return !(*this == speed);
    }

private:
    long double m_value_bit_per_ns;  // value in bit per nanosecond
};

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase>
std::ostream& operator<<(std::ostream& out, Speed<TSizeBase, TTimeBase> speed) {
    return out << speed.value();
}
