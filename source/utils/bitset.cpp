#include "utils/bitset.hpp"

#include <spdlog/fmt/fmt.h>

#include "logger/logger.hpp"

namespace sim {

BitSet32::BitSet32() : m_data(0) {}

BitSet32::BitSet32(std::uint32_t initial) : m_data(initial) {}

bool BitSet32::set_bit(std::uint32_t pos, bool value) {
    if (pos >= sizeof_bits(m_data)) {
        LOG_ERROR(fmt::format("Bit position is out of range. Max possible position is {}, but got {}", 32 - 1, pos));
        return false;
    }

    if (value) {
        m_data |= (1U << pos);
    } else {
        m_data &= ~(1U << pos);
    }
    
    return true;
}

bool BitSet32::set_range(std::uint32_t low, std::uint32_t high, std::uint32_t value) {
    if (low == high) {
        return set_bit(low, value);
    }

    if (high >= sizeof_bits(m_data) || low > high) {
        LOG_ERROR(fmt::format("Range edges error. Low and high should be less than {} and low should be <= hight. Got low = {} and high = {}", 32, low, high));
        return false;
    }

    const std::uint32_t length = high - low + 1;
    const std::uint32_t max_val = max_range_value(length);

    if (value > max_val) {
        LOG_ERROR(fmt::format("Value is out of range. Max possible value = {}, when got {}", max_val, value));
        return false;
    }

    std::uint32_t mask = max_val << low;
    m_data = (m_data & ~mask) | (value << low);

    return true;
}

std::uint32_t BitSet32::get_bit(std::uint32_t pos) const {
    if (pos >= sizeof_bits(m_data)) {
        LOG_ERROR(fmt::format("Bit position is out of range. Max possible position is {}, but got {}", 32 - 1, pos));
        return 0;
    }

    return (m_data >> pos) & 1;
}

std::uint32_t BitSet32::get_range(std::uint32_t low, std::uint32_t high) const {
    if (high >= sizeof_bits(m_data)  || low > high) {
        LOG_ERROR(fmt::format("Range edges error. Low and high should be less than {} and low should be <= hight. Got low = {} and high = {}", 32, low, high));
        return 0;
    }

    const std::uint32_t length = high - low + 1;
    std::uint32_t mask = max_range_value(length) << low;
    return (m_data & mask) >> low;
}

std::uint32_t BitSet32::get_bits() const {
    return m_data;
}

bool operator==(const BitSet32& fst, const BitSet32& snd) {
    return fst.m_data == snd.m_data;
}

void BitSet32::reset() {
    m_data = 0;
}

} // namespace sim
