#include "tcp_flow_common.hpp"

#include "utils/avg_rtt_packet_flag.hpp"

namespace sim {
BaseFlagManager TcpFlowCommon::flag_manager;
bool TcpFlowCommon::is_flag_manager_initialized = false;

void TcpFlowCommon::initialize_flag_manager() {
    if (!is_flag_manager_initialized) {
        if (!flag_manager.register_flag_by_amount(packet_type_label,
                                                  PacketType::ENUM_SIZE)) {
            throw std::runtime_error("Can not registrate packet type label");
        }
        if (!flag_manager.register_flag_by_amount(ack_ttl_label, MAX_TTL + 1)) {
            throw std::runtime_error("Can not registrate ack ttl label");
        }
        if (!register_packet_avg_rtt_flag(flag_manager)) {
            throw std::runtime_error("Can not registrate packet avg rtt label");
        }
        is_flag_manager_initialized = true;
    }
}

TcpFlowCommon::TcpFlowCommon(Id a_id, std::shared_ptr<IConnection> a_connection,
                             std::shared_ptr<IHost> a_sender,
                             std::shared_ptr<IHost> a_receiver,
                             bool a_ecn_capable)
    : id(std::move(a_id)),
      sender(a_sender),
      receiver(a_receiver),
      connection(a_connection),
      ecn_capable(a_ecn_capable) {
    initialize_flag_manager();
    if (sender.lock() == nullptr) {
        throw std::invalid_argument("Sender for TcpFlow is nullptr");
    }
    if (receiver.lock() == nullptr) {
        throw std::invalid_argument("Receiver for TcpFlow is nullptr");
    }
}

}  // namespace sim