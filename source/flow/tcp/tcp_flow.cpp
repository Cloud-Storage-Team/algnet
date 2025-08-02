#include "flow/tcp/tcp_flow.hpp"

namespace sim {

std::string TcpFlow::packet_type_label = "type";
FlagManager<std::string, PacketFlagsBase> TcpFlow::m_flag_manager;
bool TcpFlow::m_is_flag_manager_initialized = false;

}  // namespace sim
