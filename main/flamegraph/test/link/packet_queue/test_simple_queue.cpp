#include <gtest/gtest.h>

#include "abstract_queue_test.hpp"
#include "link/packet_queue/simple_packet_queue.hpp"

namespace test {

class SimpleQueueTest : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(SimpleQueueTest, PopFromEmpty) {
    TestEmpty<sim::SimplePacketQueue>(SizeByte(0));
}
TEST_F(SimpleQueueTest, TestPushEmptyPacket) {
    TestPushOnePacket<sim::SimplePacketQueue>(SizeByte(0), SizeByte(0));
}

TEST_F(SimpleQueueTest, TestPushOnePacket) {
    TestPushOnePacket<sim::SimplePacketQueue>(SizeByte(10), SizeByte(10));
}

TEST_F(SimpleQueueTest, TestOverflow) {
    TestOverflow<sim::SimplePacketQueue>(SizeByte(128));
}

}  // namespace test