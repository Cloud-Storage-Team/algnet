#include "../../source/scheduler.hpp"
#include "../../source/tcp_flow.hpp"
#include "gtest/gtest.h"
#include "utils.cpp"

namespace test {

class TCPFlowTest : public ::testing::Test {
protected:
    void SetUp() override {
        EXPECT_FALSE(sim::Scheduler::get_instance().tick());
        sender = new TestSender();
        receiver = new TestReceiver();
        flow = new sim::TCPFlow(sender, receiver, 100, 10, 5,
                                50);  // packet_size, delay_between_packets,
                                      // total_packets, delay_threshold
    }

    void TearDown() override {
        delete flow;
        delete sender;
        delete receiver;
        sim::Scheduler::get_instance().clear();
    }

    TestSender* sender;
    TestReceiver* receiver;
    sim::TCPFlow* flow;
};

TEST_F(TCPFlowTest, PacketGeneration) {
    EXPECT_TRUE(flow->try_to_generate(0));
    sim::Scheduler::get_instance().tick();
    EXPECT_TRUE(sender->has_packets());
}

TEST_F(TCPFlowTest, PacketGenerationFails) {
    EXPECT_TRUE(flow->try_to_generate(0));
    EXPECT_FALSE(flow->try_to_generate(0));
}

TEST_F(TCPFlowTest, Start) {
    flow->start(0);
    EXPECT_FALSE(sender->has_packets());
    sim::Scheduler::get_instance().tick();
    EXPECT_TRUE(sender->has_packets());
}

TEST_F(TCPFlowTest, CongestionControlLowDelay) {
    flow->update(40);  // Below delay threshold
    EXPECT_EQ(flow->m_cwnd, 2);
}

TEST_F(TCPFlowTest, CongestionControlHighDelay) {
    flow->update(60);  // Above delay threshold
    EXPECT_EQ(flow->m_cwnd, 1);
}

TEST_F(TCPFlowTest, CongestionControlThresholdHalf) {
    for (int i = 1; i < flow->m_ssthresh / 2; ++i) {
        flow->update(40);
    }
    EXPECT_EQ(flow->m_cwnd, flow->m_ssthresh / 2);
    std::uint32_t current_cwnd = flow->m_ssthresh / 2;
    flow->update(60);
    EXPECT_EQ(flow->m_cwnd, 1);
    EXPECT_EQ(flow->m_ssthresh, current_cwnd / 2);
}

TEST_F(TCPFlowTest, TotalPackets) {
    flow->start(0);
    EXPECT_TRUE(sim::Scheduler::get_instance()
                    .tick());  // Tick for start make generate events
    for (int i = 1; i < flow->m_total_packets; ++i) {
        EXPECT_TRUE(sender->has_packets());
        sender->dequeue_packet();
        EXPECT_FALSE(sender->has_packets());
        flow->update(40);
        EXPECT_TRUE(sim::Scheduler::get_instance().tick());
    }
    EXPECT_TRUE(sim::Scheduler::get_instance()
                    .tick());  // Last check before stop make generate events
    EXPECT_FALSE(flow->try_to_generate(0));
    EXPECT_FALSE(sim::Scheduler::get_instance().tick());
}

}  // namespace test
