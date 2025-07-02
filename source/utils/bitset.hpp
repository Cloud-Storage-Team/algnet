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
    std::uint32_t m_data;

    inline std::uint32_t max_range_value(std::uint32_t length) const {
        return length == sizeof_bits(m_data) ? std::numeric_limits<std::uint32_t>::max(): (static_cast<std::uint32_t>1 << length) - 1;
    }
};

} // namespace sim