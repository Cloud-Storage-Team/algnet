#pragma once

#define sizeof_bits(x) (CHAR_BIT * sizeof(x))

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <concepts>
#include <spdlog/fmt/fmt.h>

#include "logger/logger.hpp"

namespace sim {

template <typename T>
concept BitStorageType = 
    std::is_unsigned_v<T>;

template <BitStorageType BitStorage>
class BitSet {
public:
    BitSet() : m_data(0) {};
    explicit BitSet(BitStorage initial) : m_data(initial) {};

    bool set_bit(std::uint8_t pos, bool value) {
        if (pos >= sizeof_bits(m_data)) {
            LOG_ERROR(fmt::format("Bit position is out of range. Max possible position is {}, but got {}", sizeof_bits(BitStorage) - 1, pos));
            return false;
        }

        if (value) {
            m_data |= (ONE << pos);
        } else {
            m_data &= ~(ONE << pos);
        }

        return true;
    };

    bool set_range(std::uint8_t low, std::uint8_t high, BitStorage value){
        if (high >= sizeof_bits(m_data) || low > high) {
            LOG_ERROR(fmt::format("Range edges error. Low and high should be less than {} and low should be <= hight. Got low = {} and high = {}", sizeof_bits(BitStorage), low, high));
            return false;
        }

        const std::uint8_t length = high - low + 1;
        const BitStorage max_val = max_range_value(length);

        if (value > max_val) {
            LOG_ERROR(fmt::format("Value is out of range. Max possible value = {}, when got {}", max_val, value));
            return false;
        }

        BitStorage mask = max_val << low;
        m_data = (m_data & ~mask) | (value << low);

        return true;
    };

    std::uint8_t get_bit(std::uint8_t pos) const {
        if (pos >= sizeof_bits(m_data)) {
            LOG_ERROR(fmt::format("Bit position is out of range. Max possible position is {}, but got {}", sizeof_bits(BitStorage) - 1, pos));
            return 0;
        }

        return (m_data >> pos) & 1;
    };

    BitStorage get_range(std::uint8_t low, std::uint8_t high) const {
        if (high >= sizeof_bits(m_data)  || low > high) {
            LOG_ERROR(fmt::format("Range edges error. Low and high should be less than {} and low should be <= hight. Got low = {} and high = {}", sizeof_bits(BitStorage), low, high));
            return 0;
        }

        const std::uint8_t length = high - low + 1;
        BitStorage mask = max_range_value(length) << low;
        return (m_data & mask) >> low;
    };

    BitStorage get_bits() const {
        return m_data;
    };

    friend bool operator==(const BitSet& fst, const BitSet& snd) {
        return fst.m_data == snd.m_data;
    };

    void reset() {
        m_data = 0;
    };

private:
    const static inline BitStorageType ONE = 1

    BitStorage m_data;

    inline BitStorage max_range_value(std::uint8_t length) const {
        return length == sizeof_bits(BitStorage)
            ? std::numeric_limits<BitStorage>::max()
            : (static_cast<BitStorage>(1) << length) - 1;
    }
};

} // namespace sim
