#include "new_mplb_mock.hpp"

namespace test {

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
