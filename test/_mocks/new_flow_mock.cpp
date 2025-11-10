#include "new_flow_mock.hpp"

namespace test {

NewFlowMock::NewFlowMock(
    [[maybe_unused]] Id a_id, std::shared_ptr<sim::IHost> a_sender,
    std::shared_ptr<sim::IHost> a_receiver,
    [[maybe_unused]] std::shared_ptr<sim::INewMPLB> a_mplb) {
    m_context.sender = a_sender;
    m_context.receiver = a_receiver;
}

void NewFlowMock::on_packet_to_receiver(
    [[maybe_unused]] const sim::Packet& data) {
    // Mock implementation
}

void NewFlowMock::on_packet_to_sender([[maybe_unused]] const sim::Packet& ack) {
    // Mock implementation
}

void NewFlowMock::send([[maybe_unused]] std::vector<sim::Packet> packets) {
    // Mock implementation
}

const sim::FlowContext& NewFlowMock::get_context() { return m_context; }

}  // namespace test
