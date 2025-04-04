#include <gtest/gtest.h>

#include <vector>

#include "device.hpp"
#include "flow_stub.hpp"
#include "link.hpp"
#include "link_mock.hpp"
#include "packet.hpp"
#include "receiver_mock.hpp"
#include "switch.hpp"

namespace test {

class TestSender : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST_F(TestSender, test_no_inlinks) {
    auto sender = std::make_shared<sim::Sender>();
    auto receiver = std::make_shared<ReceiverMock>();
    FlowStub flow = FlowStub(sender, receiver);
    // TODO: replace with ASSERT_FALSE when process returns bool instead of
    // void
    sender->process();
    EXPECT_EQ(0, flow.get_cnt());
}

TEST_F(TestSender, test_no_packets) {
    auto sender = std::make_shared<sim::Sender>();
    auto receiver = std::make_shared<ReceiverMock>();
    FlowStub flow = FlowStub(sender, receiver);
    std::shared_ptr<LinkMock> inlink =
        std::make_shared<LinkMock>(receiver, sender);

    sender->add_inlink(inlink);

    sender->process();
    EXPECT_EQ(0, flow.get_cnt());
    // TODO: add ASSERT_FALSE when process returns bool instead of void
}

TEST_F(TestSender, test_no_flow_in_packets) {
    auto sender = std::make_shared<sim::Sender>();
    auto receiver = std::make_shared<ReceiverMock>();
    FlowStub flow = FlowStub(sender, receiver);
    std::shared_ptr<LinkMock> inlink =
        std::make_shared<LinkMock>(receiver, sender);
    
    inlink->set_ingress_packet(sim::Packet(sim::ACK, 0, nullptr));
    sender->add_inlink(inlink);

    sender->process();
    EXPECT_EQ(0, flow.get_cnt());
    // TODO: add ASSERT_FALSE when process returns bool instead of void
}

TEST_F(TestSender, test_not_ack_packet) {
    auto sender = std::make_shared<sim::Sender>();
    auto receiver = std::make_shared<ReceiverMock>();
    FlowStub flow = FlowStub(sender, receiver);
    std::shared_ptr<LinkMock> inlink =
        std::make_shared<LinkMock>(receiver, sender);
    
    inlink->set_ingress_packet(sim::Packet(sim::DATA, 0, &flow));
    sender->add_inlink(inlink);

    sender->process();
    EXPECT_EQ(0, flow.get_cnt());
    // TODO: add ASSERT_FALSE when process returns bool instead of void
}

TEST_F(TestSender, test_processing_correctness) {
    auto sender = std::make_shared<sim::Sender>();
    auto receiver = std::make_shared<ReceiverMock>();
    FlowStub flow(sender, nullptr);
    sim::Packet packet(sim::PacketType::ACK, 0, &flow);

    std::shared_ptr<LinkMock> inlink =
        std::make_shared<LinkMock>(receiver, sender);
    inlink->set_ingress_packet(packet);
    sender->add_inlink(inlink);

    // TODO: add ASSERT_FALSE when process returns bool instead of void
    sender->process();
    EXPECT_EQ(1, flow.get_cnt());
}

}  // namespace test
