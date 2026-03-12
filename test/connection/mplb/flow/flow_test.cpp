#include <gtest/gtest.h>

#include "../../../_mocks/host_gmock.hpp"
#include "network/connection/flow/tcp/new_tcp_flow.hpp"
#include "scheduler.hpp"

namespace test {

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;

struct FlowTest : public ::testing::Test {
    std::shared_ptr<NiceMock<HostGMock>> sender_mock =
        std::make_shared<NiceMock<HostGMock>>();

    std::shared_ptr<NiceMock<HostGMock>> receiver_mock =
        std::make_shared<NiceMock<HostGMock>>();

    SizeByte packet_size = SizeByte(100);

    std::shared_ptr<sim::NewTcpFlow> flow = sim::NewTcpFlow::create_shared(
        "", sim::FlowFourTuple(sim::Endpoints(sender_mock, receiver_mock)));

    bool callback_called = false;
    std::vector<sim::PacketInfo> packets_info = {sim::PacketInfo(
        sim::DataId(), packet_size,
        [this](sim::PacketAckInfo) { callback_called = true; }, TimeNs(0))};

    void SetUp() override { sim::Scheduler::get_instance().clear(); }
};

TEST_F(FlowTest, SendOnePacket) {
    EXPECT_CALL(*sender_mock, enqueue_packet(_))
        .Times(1)
        .WillOnce(Invoke([](sim::Packet packet) { packet.callback(packet); }));

    EXPECT_CALL(*receiver_mock, enqueue_packet(_))
        .Times(1)
        .WillOnce(Invoke([](sim::Packet packet) { packet.callback(packet); }));

    flow->send(packets_info);

    EXPECT_TRUE(callback_called)
        << "Callback of packet delivery have not called";

    auto flow_ctx = flow->get_context();
    ASSERT_EQ(flow_ctx.delivered_size, packet_size);
    ASSERT_EQ(flow_ctx.sent_size, packet_size);
    ASSERT_EQ(flow_ctx.retransmit_size, SizeByte(0));
}

TEST_F(FlowTest, RetransmitPacket) {
    EXPECT_CALL(*sender_mock, enqueue_packet(_))
        .Times(2)
        .WillOnce(
            Invoke([](sim::Packet) {}))  // do not deliver packet on first call
        .WillOnce(Invoke([](sim::Packet packet) { packet.callback(packet); }));

    EXPECT_CALL(*receiver_mock, enqueue_packet(_))
        .Times(1)
        .WillOnce(Invoke([](sim::Packet packet) { packet.callback(packet); }));

    flow->send(packets_info);

    while (sim::Scheduler::get_instance().tick()) {
    }

    EXPECT_TRUE(callback_called)
        << "Callback of packet delivery have not called";

    auto flow_ctx = flow->get_context();
    ASSERT_EQ(flow_ctx.delivered_size, packet_size);
    ASSERT_EQ(flow_ctx.sent_size, packet_size * 2);
    ASSERT_EQ(flow_ctx.retransmit_size, packet_size);
}

}  // namespace test
