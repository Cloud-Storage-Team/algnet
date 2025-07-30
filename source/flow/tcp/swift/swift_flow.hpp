#pragma once

#include "flow/tcp/tcp_flow.hpp"
#include "swift_cc.hpp"
namespace sim {

using TcpSwiftFlow = TcpFlow<TcpSwiftCC>;

}  // namespace sim
