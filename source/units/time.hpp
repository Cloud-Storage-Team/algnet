#pragma once

#include <concepts>
#include <cstdint>
#include <iostream>
#include <type_traits>

#include "ld_comparation.hpp"

struct Nanosecond {
    static constexpr uint64_t to_nanoseconds_multiplier = 1;
};

struct Microsecond {
    static constexpr uint64_t to_nanoseconds_multiplier = 1'000;
};

struct Milisecond {
    static constexpr uint64_t to_nanoseconds_multiplier = 1'000'000;
};

struct Second {
    static constexpr uint64_t to_nanoseconds_multiplier = 1'000'000'000;
};

// Concept for all types that might be the base for Time
template <typename T>
concept IsTimeBase = requires {
    { T::to_nanoseconds_multiplier } -> std::convertible_to<uint64_t>;
};

template <IsTimeBase TTimeBase>
class Time {
public:
    using ThisTime = Time<TTimeBase>;

    constexpr Time() : m_value_ns(0.0) {}
    template <IsTimeBase USizeBase>
    constexpr Time(Time<USizeBase> a_size)
        : m_value_ns(a_size.value_nanoseconds()) {}

    // Attention: a_value given in TTimeBase units!
    explicit constexpr Time(double a_value)
        : m_value_ns(a_value * TTimeBase::to_nanoseconds_multiplier) {}

    constexpr double value() const {
        // Round up here to get maximal time
        return m_value_ns / TTimeBase::to_nanoseconds_multiplier;
    }

    explicit constexpr operator double() const { return value(); }

    constexpr double value_nanoseconds() const { return m_value_ns; }

    constexpr ThisTime operator+(ThisTime time) const {
        return Time<Nanosecond>(m_value_ns + time.m_value_ns);
    }

    constexpr ThisTime operator-(ThisTime time) const {
        return Time<Nanosecond>(m_value_ns - time.m_value_ns);
    }

    constexpr ThisTime operator*(double mult) const {
        return Time<Nanosecond>(m_value_ns * mult);
    }

    constexpr ThisTime& operator++() {
        m_value_ns += TTimeBase::to_nanoseconds_multiplier;
        return *this;
    }

    constexpr double operator/(ThisTime time) const {
        return m_value_ns / time.value_nanoseconds();
    }

    constexpr ThisTime operator/(double value) const {
        return Time<Nanosecond>(m_value_ns / value);
    }

    constexpr void operator+=(ThisTime time) { m_value_ns += time.m_value_ns; }
    constexpr void operator-=(ThisTime time) { m_value_ns -= time.m_value_ns; }

    constexpr void operator*=(double mult) { m_value_ns *= mult; }

    constexpr void operator/=(double mult) { m_value_ns /= mult; }

    bool constexpr operator<(ThisTime time) const {
        return m_value_ns < time.m_value_ns;
    }

    bool constexpr operator>(ThisTime time) const {
        return m_value_ns > time.m_value_ns;
    }

    bool constexpr operator==(ThisTime time) const {
        return equal(m_value_ns, time.value_nanoseconds());
    }

    bool constexpr operator!=(ThisTime time) const { return !operator==(time); }

private:
    double m_value_ns;  // Time in nanoseconds
};

template <IsTimeBase TTimeBase>
std::ostream& operator<<(std::ostream& out, Time<TTimeBase> time) {
    return out << time.value();
}
