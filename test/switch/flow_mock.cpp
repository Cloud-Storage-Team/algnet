#include "flow_mock.hpp"

namespace test {
FlowMock::FlowMock(std::shared_ptr<sim::IReceiver> m_receiver)
    : m_receiver(m_receiver) {}

void FlowMock::start(std::uint32_t time) {}
bool FlowMock::try_to_generate(std::uint32_t packet_size) { return false; }

// Update the internal state according to some congestion control algorithm
// Call try_to_generate upon the update
void FlowMock::update() {}
std::shared_ptr<sim::IReceiver> FlowMock::get_destination() const {
    return m_receiver;
}

}  // namespace test
