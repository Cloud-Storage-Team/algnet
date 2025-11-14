#include "new_flow_mock.hpp"

namespace test {

NewFlowMock::NewFlowMock(
    Id a_id, [[maybe_unused]] std::shared_ptr<sim::IHost> a_sender,
    [[maybe_unused]] std::shared_ptr<sim::IHost> a_receiver,
    [[maybe_unused]] sim::INewMPLB& a_mplb)
    : m_id(std::move(a_id)), m_mplb(a_mplb) {
    m_context.sender = a_sender;
    m_context.receiver = a_receiver;
}

Id NewFlowMock::get_id() const { return m_id; }

void NewFlowMock::on_packet([[maybe_unused]] const sim::Packet& packet) {
    // Mock implementation
}

void NewFlowMock::send([[maybe_unused]] std::vector<sim::Packet> packets) {
    // Mock implementation
}

const sim::FlowContext& NewFlowMock::get_context() { return m_context; }

}  // namespace test
