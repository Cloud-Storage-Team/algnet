#pragma once

#include <cstdint>
#include <stdexcept>

namespace sim {

class BitSet32 {
public:
    BitSet32();
    explicit BitSet32(std::uint32_t initial);

    bool set_bit(std::uint32_t pos, bool value);
    bool set_range(std::uint32_t low, std::uint32_t high, std::uint32_t value);

    std::uint32_t get_bit(std::uint32_t pos) const;
    std::uint32_t get_range(std::uint32_t low, std::uint32_t high) const;
    std::uint32_t get_bits() const;

    friend bool operator==(const BitSet32& fst, const BitSet32& snd);

    void reset();

private:
    const static std::uint32_t MAX_POS = 32;
    std::uint32_t m_data;

    inline std::uint32_t max_range_value(std::uint32_t length) const {
        return length == MAX_POS ? 0xFFFFFFFF : (1U << length) - 1;
    }
};

} // namespace sim