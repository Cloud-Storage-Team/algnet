#pragma once
#include <memory>

#include "metrics/packet_reordering/simple_packet_reordering.hpp"
#include "tcp_flow_common.hpp"
#include "utils/packet_num_monitor.hpp"

namespace sim {
class TcpFlowReceiver {
public:
    explicit TcpFlowReceiver(TcpCommonPtr a_common);

    void process_data_packet(Packet packet);

private:
    Packet create_ack(Packet data);

    TcpCommonPtr m_common;
    PacketNumMonitor m_data_packets_monitor;
    SimplePacketReordering m_packet_reordering;
};
}  // namespace sim