#pragma once
#include <memory>

#include "tcp_common.hpp"

namespace sim {
class TcpReceiver {
public:
    explicit TcpReceiver(TcpCommonPtr a_common);

public:
    TcpCommonPtr m_common;
};
}  // namespace sim