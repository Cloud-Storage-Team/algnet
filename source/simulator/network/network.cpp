#include "network.hpp"

namespace sim {

Network::Network(NetworkContext a_ctx) : m_ctx(std::move(a_ctx)) {}

const NetworkContext& Network::get_context() const { return m_ctx; }
}  // namespace sim
