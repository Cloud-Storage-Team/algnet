#pragma once
#include <memory>

#include "metrics/packet_reordering/simple_packet_reordering.hpp"
#include "tcp_flow_common.hpp"

namespace sim {
class TcpFlowReceiver {
public:
    explicit TcpFlowReceiver(TcpCommonPtr a_common);

    void process_data_packet(Packet packet);

private:
    Packet create_ack(Packet data);

    TcpCommonPtr m_common;
    SimplePacketReordering m_packet_reordering;
};
}  // namespace sim