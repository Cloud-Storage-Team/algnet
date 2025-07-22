#pragma once

#include <concepts>
#include <cstdint>
#include <iostream>
#include <type_traits>

#include "ld_comparation.hpp"

struct Nanosecond {
    static constexpr uint64_t to_nanoseconds_multiplier = 1;
    static constexpr std::string name = "ns";
};

struct Microsecond {
    static constexpr uint64_t to_nanoseconds_multiplier = 1'000;
    static constexpr std::string name = "mcs";
};

struct Milisecond {
    static constexpr uint64_t to_nanoseconds_multiplier = 1'000'000;
    static constexpr std::string name = "mls";
};

struct Second {
    static constexpr uint64_t to_nanoseconds_multiplier = 1'000'000'000;
    static constexpr std::string name = "s";
};

// Concept for all types that might be the base for Time
template <typename T>
concept IsTimeBase = requires {
    { T::to_nanoseconds_multiplier } -> std::convertible_to<uint64_t>;
    { T::name } -> std::convertible_to<std::string>;
};

template <IsTimeBase TTimeBase>
class Time {
public:
    using ThisTime = Time<TTimeBase>;
    static constexpr std::string unit_name = TTimeBase::name;

    template <IsTimeBase USizeBase>
    constexpr Time(Time<USizeBase> a_size)
        : m_value_ns(a_size.get_nanoseconds()) {}

    // Attention: a_value given in TTimeBase units!
    explicit constexpr Time(long double a_value)
        : m_value_ns(a_value * TTimeBase::to_nanoseconds_multiplier) {}

    constexpr long double value() const {
        // Round up here to get maximal time
        return m_value_ns / TTimeBase::to_nanoseconds_multiplier;
    }

    constexpr long double get_nanoseconds() const { return m_value_ns; }

    constexpr ThisTime operator+(ThisTime time) const {
        return Time<Nanosecond>(m_value_ns + time.m_value_ns);
    }

    constexpr ThisTime operator-(ThisTime time) const {
        return Time<Nanosecond>(m_value_ns - time.m_value_ns);
    }

    constexpr ThisTime operator*(long double mult) const {
        return Time<Nanosecond>(m_value_ns * mult);
    }

    constexpr ThisTime operator++() {
        m_value_ns += TTimeBase::to_nanoseconds_multiplier;
        return *this;
    }

    constexpr long double operator/(ThisTime time) const {
        return m_value_ns / time.get_nanoseconds();
    }

    constexpr void operator+=(ThisTime time) { m_value_ns += time.m_value_ns; }
    constexpr void operator-=(ThisTime time) { m_value_ns -= time.m_value_ns; }

    constexpr void operator*=(long double mult) { m_value_ns *= mult; }

    constexpr void operator/=(long double mult) { m_value_ns /= mult; }

    bool constexpr operator<(ThisTime time) const {
        return m_value_ns < time.m_value_ns;
    }

    bool constexpr operator>(ThisTime time) const {
        return m_value_ns > time.m_value_ns;
    }

    bool constexpr operator==(ThisTime time) const {
        return equal(m_value_ns, time.get_nanoseconds());
    }

    bool constexpr operator!=(ThisTime time) const { return !operator==(time); }

private:
    long double m_value_ns;  // Time in nanoseconds
};

template <IsTimeBase TTimeBase>
std::ostream& operator<<(std::ostream& out, Time<TTimeBase> time) {
    return out << time.value();
}
