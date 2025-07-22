#pragma once
#include <concepts>
#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>

struct Bit {
    static constexpr uint64_t to_bit_multiplier = 1;
};

struct Byte {
    static constexpr uint64_t to_bit_multiplier = (1ull << 3);
};

struct KBit {
    static constexpr uint64_t to_bit_multiplier = (1ull << 10);
};

struct KByte {
    static constexpr uint64_t to_bit_multiplier = (1ull << 13);
};

struct MBit {
    static constexpr uint64_t to_bit_multiplier = (1ull << 20);
};

struct MByte {
    static constexpr uint64_t to_bit_multiplier = (1ull << 23);
};

struct GBit {
    static constexpr uint64_t to_bit_multiplier = (1ull << 30);
};

struct GByte {
    static constexpr uint64_t to_bit_multiplier = (1ull << 33);
};

// Concept for all types that might be the base for Size
template <typename T>
concept IsSizeBase = requires {
    { T::to_bit_multiplier } -> std::convertible_to<uint64_t>;
};

template <IsSizeBase TSizeBase>
class Size {
public:
    using ThisSize = Size<TSizeBase>;

    template <IsSizeBase USizeBase>
    constexpr Size(Size<USizeBase> a_size) : m_value_bits(a_size.get_bits()) {}

    // Attention: a_value given in TSizeBase units!
    explicit constexpr Size(uint64_t a_value)
        : m_value_bits(a_value * TSizeBase::to_bit_multiplier) {}

    constexpr uint64_t value() const {
        // Round up here to get maximal size
        return (m_value_bits + TSizeBase::to_bit_multiplier - 1) /
               TSizeBase::to_bit_multiplier;
    }

    constexpr uint64_t get_bits() const { return m_value_bits; }

    constexpr ThisSize operator+(ThisSize size) const {
        return Size<Bit>(m_value_bits + size.m_value_bits);
    }

    constexpr ThisSize operator-(ThisSize size) const {
        return Size<Bit>(m_value_bits - size.m_value_bits);
    }

    constexpr ThisSize operator*(size_t mult) const {
        return Size<Bit>(m_value_bits * mult);
    }

    constexpr double operator/(ThisSize size) const {
        return m_value_bits / (double)size.get_bits();
    }

    constexpr void operator+=(ThisSize size) {
        m_value_bits += size.m_value_bits;
    }
    constexpr void operator-=(ThisSize size) {
        m_value_bits -= size.m_value_bits;
    }

    constexpr void operator*=(size_t mult) { m_value_bits *= mult; }

    constexpr void operator/=(size_t mult) {
        m_value_bits = (m_value_bits + mult - 1) / mult;
    }

    bool constexpr operator<(ThisSize size) const {
        return m_value_bits < size.m_value_bits;
    }

    bool constexpr operator>(ThisSize size) const {
        return m_value_bits > size.m_value_bits;
    }

    bool constexpr operator==(ThisSize size) const {
        return m_value_bits == size.m_value_bits;
    }

    bool constexpr operator!=(ThisSize size) const {
        return m_value_bits != size.m_value_bits;
    }

private:
    uint64_t m_value_bits;  // Size in bits
};

template <IsSizeBase TSizeBase>
std::ostream& operator<<(std::ostream& out, Size<TSizeBase> size) {
    return out << size.value();
}
