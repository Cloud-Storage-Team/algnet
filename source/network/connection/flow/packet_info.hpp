#pragma once
#include "network/connection/data.hpp"
#include "packet_ack_info.hpp"
#include "types.hpp"

namespace sim {

using PacketCallback = std::function<void(PacketAckInfo)>;

struct PacketInfo {
    DataId id;
    SizeByte packet_size;
    PacketCallback callback;
    TimeNs generated_time;

    std::string to_string() const;
};
}  // namespace sim
