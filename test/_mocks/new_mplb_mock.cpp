#include "new_mplb_mock.hpp"

namespace test {

NewMPLBMock::NewMPLBMock(std::shared_ptr<sim::INewFlowFactory> flow_factory) {
    m_flow_factory = std::move(flow_factory);
}

void NewMPLBMock::on_ack(
    std::shared_ptr<const sim::INewFlow> flow,
    [[maybe_unused]] const sim::Packet& ack,
    [[maybe_unused]] std::vector<PacketNum> confirmed_packet_nums) {
    // Mock implementation
}

SizeByte NewMPLBMock::send_data([[maybe_unused]] sim::Data data) {
    return SizeByte(100);
}

}  // namespace test
