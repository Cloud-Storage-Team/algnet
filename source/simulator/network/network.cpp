#include "network.hpp"

namespace sim {

Network::Network(SimulationContext a_ctx) : m_ctx(std::move(a_ctx)) {}

const SimulationContext& Network::get_context() const { return m_ctx; }
}  // namespace sim
