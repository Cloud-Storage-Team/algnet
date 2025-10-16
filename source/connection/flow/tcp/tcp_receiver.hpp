#pragma once
#include <memory>

#include "metrics/packet_reordering/simple_packet_reordering.hpp"
#include "tcp_common.hpp"

namespace sim {
class TcpReceiver {
public:
    explicit TcpReceiver(TcpCommonPtr a_common);

public:
    TcpCommonPtr m_common;
    SimplePacketReordering m_packet_reordering;
};
}  // namespace sim