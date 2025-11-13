#pragma once

#include <spdlog/fmt/fmt.h>

#include <unordered_map>

#include "bitset.hpp"
#include "logger/logger.hpp"
#include "utils/str_expected.hpp"

namespace sim {

template <typename FlagId, BitStorageType BitStorage>
class FlagManager {
public:
    FlagManager() : m_next_pos(0), m_bits(0) {}

    [[nodiscard]] bool register_flag_by_amount(FlagId id,
                                               BitStorage different_values) {
        return register_flag_by_length(
            id, required_bits_for_values(different_values));
    }

    [[nodiscard]] bool register_flag_by_length(FlagId id,
                                               BitStorage flag_length) {
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

        m_flag_manager[id] = FlagInfo{m_next_pos, flag_length, false};
        m_next_pos += flag_length;
        return true;
    }

    [[nodiscard]] utils::StrExpected<void> set_flag(FlagId id,
                                                            BitStorage value) {
        auto it = m_flag_manager.find(id);
        if (it == m_flag_manager.end()) {
            return std::unexpected(
                fmt::format("Flag with id '{}' was not registred.", id));
        }

        FlagInfo& info = it->second;
        m_bits.set_range(info.start, info.start + info.length - 1, value);
        info.is_set = true;
        return {};
    }

    [[nodiscard]] utils::StrExpected<BitStorage> get_flag(FlagId id) const {
        auto it = m_flag_manager.find(id);
        if (it == m_flag_manager.end()) {
            return std::unexpected(
                fmt::format("Flag with id '{}' was not registred.", id));
        }
        const FlagInfo& info = it->second;
        if (!info.is_set) {
            return std::unexpected(
                fmt::format("Flag with id '{}' was not set.", id));
        }
        return m_bits.get_range(info.start, info.start + info.length - 1);
    }

    BitSet<BitStorage> get_bit_storage() const { return m_bits; }

    void reset() {
        m_next_pos = 0;
        m_bits = BitSet<BitStorage>(0);
        m_flag_manager.clear();
    }

private:
    struct FlagInfo {
        BitStorage start;
        BitStorage length;
        bool is_set;
    };

    inline BitStorage required_bits_for_values(BitStorage max_values) {
        if (max_values <= 1) {
            return 0;
        }

        max_values--;
        BitStorage result = 0;
        while (max_values > 0) {
            ++result;
            max_values >>= (BitStorage)1;
        }

        return result;
    }

    BitStorage m_next_pos = 0;
    BitSet<BitStorage> m_bits;
    std::unordered_map<FlagId, FlagInfo> m_flag_manager;
};

using BaseFlagType = std::string;
using BaseFlagManager = FlagManager<BaseFlagType, PacketFlagsBase>;

}  // namespace sim