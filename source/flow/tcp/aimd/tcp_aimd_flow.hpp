#pragma once

#include "flow/tcp/tcp_flow.hpp"
#include "tcp_aimd_cc.hpp"
namespace sim {

using TcpAIMD_Flow = TcpFlow<TcpAIMD_CC>;

}  // namespace sim
