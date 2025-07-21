#pragma once
#include <concepts>
#include <cstdint>
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

struct GBit {
    static constexpr uint64_t to_bit_multiplier = (1ull << 20);
};

struct GByte {
    static constexpr uint64_t to_bit_multiplier = (1ull << 23);
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

    // Attention: a_value given in TSizeBase units!
    constexpr Size(uint64_t a_value)
        : m_value_bits(a_value * TSizeBase::to_bit_multiplier) {}

    constexpr operator uint64_t() const {
        return m_value_bits / TSizeBase::to_bit_multiplier;
    }

    constexpr void operator+=(ThisSize size) {
        m_value_bits += size.m_value_bits;
    }
    constexpr void operator-=(ThisSize size) {
        m_value_bits -= size.m_value_bits;
    }

private:
    uint64_t m_value_bits;  // Size in bits
};