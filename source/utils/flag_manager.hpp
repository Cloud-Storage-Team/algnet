#pragma once

#include <spdlog/fmt/fmt.h>

#include <unordered_map>

#include "bitset.hpp"
#include "logger/logger.hpp"
#include "packet/packet.hpp"

namespace sim {

template <typename FlagId, BitStorageType BitStorage>
class FlagManager {
public:
    FlagManager() : m_next_pos(0) {}

    bool register_flag_by_amount(FlagId id, BitStorage different_values) {
        return register_flag_by_length(
            id, required_bits_for_values(different_values));
    }

    bool register_flag_by_length(FlagId id, BitStorage flag_length) {
        if (flag_length == 0) {
            LOG_ERROR("Passed zero flag length, should be at least 1");
            return false;
        }

        if (m_next_pos + flag_length > sizeof_bits(BitStorage)) {
            LOG_ERROR(
                fmt::format("Partition position is out of range. Max possible "
                            "position is {}, but got {}",
                            sizeof_bits(BitStorage), m_next_pos + flag_length));
            return false;
        }

        if (m_flag_manager.find(id) != m_flag_manager.end()) {
            LOG_ERROR(
                fmt::format("Flag with same id '{}' already exists.", id));
            return false;
        }

        m_flag_manager[id] = FlagInfo{m_next_pos, flag_length};
        m_next_pos += flag_length;
        return true;
    }

    void set_flag(Packet& packet, FlagId id, BitStorage value) {
        auto it = m_flag_manager.find(id);
        if (it == m_flag_manager.end()) {
            LOG_ERROR(fmt::format("Flag with id '{}' not found", id));
            return;
        }

        const FlagInfo& info = it->second;
        packet.flags.set_range(info.start, info.start + info.length - 1, value);
    }

    BitStorage get_flag(const Packet& packet, FlagId id) const {
        auto it = m_flag_manager.find(id);
        if (it == m_flag_manager.end()) {
            LOG_ERROR(fmt::format("Flag with id '{}' not found.", id));
            return 0;
        }
        const FlagInfo& info = it->second;
        return packet.flags.get_range(info.start, info.start + info.length - 1);
    }

    void reset() {
        m_next_pos = 0;
        m_flag_manager.clear();
    }

private:
    struct FlagInfo {
        BitStorage start;
        BitStorage length;
    };

    inline BitStorage required_bits_for_values(BitStorage max_values) {
        if (max_values <= 1) {
            return 0;
        }

        max_values--;
        return sizeof_bits(max_values) - __builtin_clz(max_values);
    }

    BitStorage m_next_pos = 0;
    std::unordered_map<FlagId, FlagInfo> m_flag_manager;
};

}  // namespace sim