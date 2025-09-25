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

inline bool registate_packet_avg_rtt_flag(BaseFlagManager& flag_manager) {
    return flag_manager.register_flag_by_length(packet_avg_rtt_label,
                                                sizeof_bits(AvgRttCastType));
}

inline void set_avg_rtt_label(BaseFlagManager& flag_manager, BaseBitset& bitset,
                              TimeNs rtt) {
    AvgRttCastType value = rtt.value_nanoseconds();
    AvgRttFlagType casted_value = *reinterpret_cast<AvgRttFlagType*>(&value);
    flag_manager.set_flag(bitset, packet_avg_rtt_label, casted_value);
}

inline TimeNs get_avg_rtt_label(const BaseFlagManager& flag_manager,
                                BaseBitset& bitset) {
    AvgRttFlagType casted_value =
        flag_manager.get_flag(bitset, packet_avg_rtt_label);
    return TimeNs(*reinterpret_cast<AvgRttCastType*>(&casted_value));
}

}  // namespace sim