#include "tcp_receiver.hpp"

namespace sim {
TcpReceiver::TcpReceiver(TcpCommonPtr a_common,
                         std::shared_ptr<IHost> a_receiver)
    : m_common(a_common), m_receiver(a_receiver) {
    if (m_receiver.lock() == nullptr) {
        throw std::invalid_argument("Receiver for TcpFlow is nullptr");
    }
}
}  // namespace sim