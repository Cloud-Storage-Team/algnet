#pragma once
#include <string>

#include "flag_manager.hpp"

namespace sim {

static std::string packet_avg_rtt_label = "avg_rtt";
// to this type real rtt value will be casted
using AvgRttCastType = float;
// to this type casted value will be transormed using reinterpred_cast
using AvgRttFlagType = uint32_t;

static_assert(sizeof(AvgRttCastType) == sizeof(AvgRttFlagType),
              "Rtt cast type and flag type should have same size");

inline bool register_packet_avg_rtt_flag(BaseFlagManager& flag_manager) {
    return flag_manager.register_flag_by_length(packet_avg_rtt_label,
                                                sizeof_bits(AvgRttCastType));
}

[[nodiscard]] inline utils::StrExpected<void> set_avg_rtt_flag(
    BaseFlagManager& flag_manager, TimeNs rtt) {
    AvgRttCastType value = rtt.value_nanoseconds();
    AvgRttFlagType casted_value = std::bit_cast<AvgRttFlagType>(value);
    return flag_manager.set_flag(packet_avg_rtt_label, casted_value);
}

[[nodiscard]] inline utils::StrExpected<TimeNs> get_avg_rtt_label(
    const BaseFlagManager& flag_manager) {
    auto exp_flag = flag_manager.get_flag(packet_avg_rtt_label);
    if (!exp_flag.has_value()) {
        return std::unexpected(exp_flag.error());
    }
    AvgRttFlagType casted_value = exp_flag.value();
    AvgRttCastType value = std::bit_cast<AvgRttCastType>(casted_value);
    return TimeNs(value);
}

}  // namespace sim