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

TEST_F(TestSender, test_no_packets_to_send) {
    auto sender = std::make_shared<sim::Sender>();
    auto receiver = std::make_shared<ReceiverMock>();
    FlowStub flow = FlowStub(sender, receiver);
    std::shared_ptr<LinkMock> outlink =
        std::make_shared<LinkMock>(sender, receiver);
    sender->update_routing_table(receiver, outlink);

    sender->send_data();
    EXPECT_EQ(0, outlink->get_arrived_packets().size());
}

TEST_F(TestSender, test_no_outlink) {
    auto sender = std::make_shared<sim::Sender>();
    auto receiver = std::make_shared<ReceiverMock>();
    FlowStub flow = FlowStub(sender, receiver);
    sender->enqueue_packet(sim::Packet(sim::DATA, 0, &flow));
    
    sender->send_data();
    // TODO: add ASSERT_FALSE when send_data returns bool instead of void
}

TEST_F(TestSender, test_correct_sending) {
    auto sender = std::make_shared<sim::Sender>();
    auto receiver = std::make_shared<ReceiverMock>();
    FlowStub flow = FlowStub(sender, receiver);
    std::shared_ptr<LinkMock> outlink =
        std::make_shared<LinkMock>(sender, receiver);
    sender->update_routing_table(receiver, outlink);

    auto packet = sim::Packet(sim::DATA, 0, &flow);
    sender->enqueue_packet(packet);
    
    sender->send_data();
    EXPECT_EQ(1, outlink->get_arrived_packets().size());
    EXPECT_EQ(packet, outlink->get_arrived_packets()[0]);
}

}  // namespace test
