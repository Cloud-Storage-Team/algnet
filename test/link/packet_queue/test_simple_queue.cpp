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
    TestPopEmpty<sim::SimplePacketQueue>(0);
}

}  // namespace test