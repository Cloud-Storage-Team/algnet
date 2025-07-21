#pragma once

#include <concepts>
#include <cstdint>
#include <iostream>
#include <type_traits>

struct Nanoseconds {
    static constexpr uint64_t to_nanoseconds_multiplier = 1;
};

struct Microseconds {
    static constexpr uint64_t to_nanoseconds_multiplier = 1'000;
};

struct Miliseconds {
    static constexpr uint64_t to_nanoseconds_multiplier = 1'000'000;
};

struct Seconds {
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

    template <IsTimeBase USizeBase>
    constexpr Time(Time<USizeBase> a_size)
        : m_value_ns(a_size.get_nanoseconds()) {}

    // Attention: a_value given in TTimeBase units!
    explicit constexpr Time(uint64_t a_value)
        : m_value_ns(a_value * TTimeBase::to_nanoseconds_multiplier) {}

    constexpr uint64_t value() const {
        // Round up here to get maximal time
        return (m_value_ns + TTimeBase::to_nanoseconds_multiplier - 1) /
               TTimeBase::to_nanoseconds_multiplier;
    }

    constexpr uint64_t get_nanoseconds() const { return m_value_ns; }

    constexpr ThisTime operator+(ThisTime time) const {
        return Time<Nanoseconds>(m_value_ns + time.m_value_ns);
    }

    constexpr ThisTime operator-(ThisTime time) const {
        return Time<Nanoseconds>(m_value_ns - time.m_value_ns);
    }

    constexpr ThisTime operator*(size_t mult) const {
        return Time<Nanoseconds>(m_value_ns * mult);
    }

    constexpr ThisTime operator++() {
        m_value_ns += TTimeBase::to_nanoseconds_multiplier;
        return *this;
    }

    constexpr double operator/(ThisTime time) const {
        return m_value_ns / (double)time.get_nanoseconds();
    }

    constexpr void operator+=(ThisTime time) { m_value_ns += time.m_value_ns; }
    constexpr void operator-=(ThisTime time) { m_value_ns -= time.m_value_ns; }

    constexpr void operator*=(size_t mult) { m_value_ns *= mult; }

    constexpr void operator/=(size_t mult) {
        m_value_ns = (m_value_ns + mult - 1) / mult;
    }

    bool constexpr operator<(ThisTime time) const {
        return m_value_ns < time.m_value_ns;
    }

    bool constexpr operator<=(ThisTime time) const {
        return m_value_ns <= time.m_value_ns;
    }

    bool constexpr operator>(ThisTime time) const {
        return m_value_ns > time.m_value_ns;
    }

    bool constexpr operator>=(ThisTime time) const {
        return m_value_ns >= time.m_value_ns;
    }

    bool constexpr operator==(ThisTime time) const {
        return m_value_ns == time.m_value_ns;
    }

    bool constexpr operator!=(ThisTime time) const {
        return m_value_ns != time.m_value_ns;
    }

private:
    uint64_t m_value_ns;  // Time in nanoseconds
};

template <IsTimeBase TTimeBase>
std::ostream& operator<<(std::ostream& out, Time<TTimeBase> time) {
    return out << time.value();
}
