#pragma once

#include "bitset.hpp"
#include "packet.hpp"

#include <unordered_map>
#include <spdlog/fmt/fmt.h>

#include "logger/logger.hpp"

namespace sim {

template <typename FlagId>
class FlagManager {
public:
    FlagManager() : m_next_pos(0) {}

    bool register_flag_by_amount(FlagId id, std::uint32_t different_values) {
        return register_flag_by_length(id, required_bits_for_values(different_values));
    }

    bool register_flag_by_length(FlagId id, std::uint32_t flag_length) {
        if (flag_length == 0 || flag_length > 32) {
            LOG_ERROR(fmt::format("Incorrect flag length. Max possible length is {}, flag length should be more than 0. Got {}", 32, flag_length));
            return false;
        }

        if (m_next_pos + flag_length > 32) {
            LOG_ERROR(fmt::format("Partition position is out of range. Max possible position is {}, but got {}", 32, m_next_pos + flag_length));
            return false;
        }

        if (m_flags.find(id) != m_flags.end()) {
            LOG_ERROR(fmt::format("Flag already exists. Flag id: {}", id));
            return false;
        }

        m_flags[id] = FlagInfo{ m_next_pos, flag_length };
        m_next_pos += flag_length;
        return true;
    }

    void set_flag(Packet& packet, FlagId id, std::uint32_t value) {
        auto it = m_flags.find(id);
        if (it == m_flags.end()) {
            LOG_ERROR(fmt::format("Flag was not registered. Flag id: {}", id));
            return;
        }

        const FlagInfo& info = it->second;
        packet.flags.set_range(info.start, info.start + info.length - 1, value);
    }

    std::uint32_t get_flag(const Packet& packet, FlagId id) const {
        auto it = m_flags.find(id);
        if (it == m_flags.end()) {
            LOG_ERROR(fmt::format("Flag was not registered. Flag id: {}", id));
            return 0;
        }
        const FlagInfo& info = it->second;
        return packet.flags.get_range(info.start, info.start + info.length - 1);
    }

    void reset() {
        m_next_pos = 0;
        m_flags.clear();
    }

private:
    struct FlagInfo {
        std::uint32_t start;
        std::uint32_t length;
    };

    inline std::uint32_t required_bits_for_values(std::uint32_t max_values) {
        if (max_values <= 1) {
            return 0;
        }

        max_values--;
        return sizeof_bits(max_values) - __builtin_clz(max_values);
    }

    std::uint32_t m_next_pos = 0;
    std::unordered_map<FlagId, FlagInfo> m_flags;
};

} // namespace sim