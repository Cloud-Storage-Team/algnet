#include <gtest/gtest.h>

#include "../../../_mocks/flow_gmock.hpp"
#include "connection/flow/tcp/new_tcp_flow.hpp"

namespace test {

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;

class FlowTest : public ::testing::Test {};

TEST_F(FlowTest, SendOnePacket) {
    std::shared_ptr<NiceMock<HostGMock>> sender_mock =
        std::make_shared<NiceMock<HostGMock>>();
    EXPECT_CALL(*sender_mock, enqueue_packet(_))
        .Times(1)
        .WillOnce(Invoke([](sim::Packet packet) { packet.callback(packet); }));
}

}  // namespace test
