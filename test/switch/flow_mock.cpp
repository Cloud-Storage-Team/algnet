#include "flow_mock.hpp"

namespace test {

FlowMock::FlowMock(std::shared_ptr<sim::IReceiver> m_receiver)
    : m_receiver(m_receiver) {}

void FlowMock::start(std::uint32_t time) {}
Time FlowMock::try_to_generate() { return 0; }

void FlowMock::update() {}
std::shared_ptr<sim::ISender> FlowMock::get_sender() const {
    return nullptr;
}
std::shared_ptr<sim::IReceiver> FlowMock::get_receiver() const {
    return m_receiver;
}

}  // namespace test
