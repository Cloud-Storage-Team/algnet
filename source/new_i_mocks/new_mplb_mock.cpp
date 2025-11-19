#include "new_mplb_mock.hpp"

namespace test {

NewMPLBMock::NewMPLBMock([[maybe_unused]] std::shared_ptr<sim::IHost> a_src,
                         [[maybe_unused]] std::shared_ptr<sim::IHost> a_dest,
                         std::unique_ptr<sim::INewFlowFactory> a_flow_factory,
                         std::unique_ptr<sim::ITcpCCFactory> a_cc_factory,
                         [[maybe_unused]] sim::MPLBType a_mplb_type) {
    m_flow_factory = std::move(a_flow_factory);
    m_cc_factory = std::move(a_cc_factory);
    m_flow_factory->set_mplb_ack_cb(
        [this](std::shared_ptr<const sim::INewFlow> flow,
               const sim::Packet& ack,
               std::vector<PacketNum> confirmed_packet_nums) {
            this->on_ack(flow, ack, confirmed_packet_nums);
        });
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

void NewMPLBMock::set_conn_confirm_data_cb(sim::ConnConfirmDataCallback cb) {
    m_confirm_data_cb = cb;
}

}  // namespace test
