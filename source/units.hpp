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

template <typename T>
concept HasToBitMultiplier = requires {
    { T::to_bit_multiplier } -> std::convertible_to<uint64_t>;
};

template <HasToBitMultiplier TSizeBase>
class TemplateSize {
public:
    using Size = TemplateSize<TSizeBase>;

    // Attention: a_value given in TSizeByte units!
    constexpr TemplateSize(uint64_t a_value)
        : m_value_bits(a_value * TSizeBase::to_bit_multiplier) {}

    constexpr operator uint64_t() const {
        return m_value_bits / TSizeBase::to_bit_multiplier;
    }

    constexpr void operator+=(Size size) { m_value_bits += size.m_value_bits; }
    constexpr void operator-=(Size size) { m_value_bits -= size.m_value_bits; }

private:
    uint64_t m_value_bits;  // Size in bits
};