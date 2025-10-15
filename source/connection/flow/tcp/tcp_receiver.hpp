#pragma once
#include <memory>

#include "tcp_common.hpp"

namespace sim {
class TcpReceiver {
public:
    TcpReceiver(TcpCommonPtr a_common, std::shared_ptr<IHost> a_receiver);

public:
    TcpCommonPtr m_common;
    std::weak_ptr<IHost> m_receiver;
};
}  // namespace sim