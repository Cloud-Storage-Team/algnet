#include "new_mplb_mock.hpp"

namespace test {

NewMPLBMock::NewMPLBMock([[maybe_unused]] std::shared_ptr<sim::IHost> a_src,
                         [[maybe_unused]] std::shared_ptr<sim::IHost> a_dest,
                         std::unique_ptr<sim::INewFlowFactory> flow_factory,
                         std::unique_ptr<sim::ITcpCCFactory> cc_factory) {
    m_flow_factory = std::move(flow_factory);
    m_cc_factory = std::move(cc_factory);
}

void NewMPLBMock::on_ack(
    [[maybe_unused]] std::shared_ptr<const sim::INewFlow> flow,
    [[maybe_unused]] const sim::Packet& ack,
    [[maybe_unused]] std::vector<PacketNum> confirmed_packet_nums) {
    // Mock implementation
}

SizeByte NewMPLBMock::send_data([[maybe_unused]] sim::Data data) {
    return SizeByte(100);
}

}  // namespace test
