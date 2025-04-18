#include "flow_mock.hpp"

namespace test {
FlowMock::FlowMock(std::shared_ptr<sim::IReceiver> m_receiver)
    : m_receiver(m_receiver) {}

void FlowMock::start(std::uint32_t time) {}
bool FlowMock::try_to_generate(std::uint32_t packet_size) { return false; }

void FlowMock::update(std::uint32_t delay) {}
std::shared_ptr<sim::ISender> FlowMock::get_source() const noexcept { 
    return nullptr; 
}
std::shared_ptr<sim::IReceiver> FlowMock::get_destination() const noexcept { 
    return m_receiver; 
}

}  // namespace test
