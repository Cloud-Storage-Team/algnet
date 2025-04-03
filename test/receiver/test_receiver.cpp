#include <gtest/gtest.h>

#include "flow_mock.hpp"
#include "link_mock.hpp"
#include "packet.hpp"
#include "receiver.hpp"
#include "sender_mock.hpp"
#include "switch.hpp"

namespace test_receiver {

class TestReceiver : public ::testing::Test {
protected:
    void SetUp() override { receiver = std::make_shared<sim::Receiver>(); }

    std::shared_ptr<sim::Receiver> receiver;
};

TEST_F(TestReceiver, Constructor) { EXPECT_NE(receiver, nullptr); }

TEST_F(TestReceiver, AddInlinkValid) {
    auto from = std::make_shared<sim::Switch>();
    auto to = receiver;
    auto link = std::make_shared<LinkMock>(from, to);

    receiver->add_inlink(link);

    auto next_inlink = receiver->next_inlink();
    EXPECT_EQ(next_inlink, link);
}

TEST_F(TestReceiver, AddInlinkNullptr) {
    receiver->add_inlink(nullptr);

    auto next_inlink = receiver->next_inlink();
    EXPECT_EQ(next_inlink, nullptr);
}

TEST_F(TestReceiver, AddInlinkInvalid) {
    auto from = std::make_shared<sim::Switch>();
    auto to = std::make_shared<sim::Receiver>();
    auto link = std::make_shared<LinkMock>(from, to);

    receiver->add_inlink(link);

    auto next_inlink = receiver->next_inlink();
    EXPECT_EQ(next_inlink, nullptr);
}

TEST_F(TestReceiver, UpdateRoutingTableValid) {
    auto to = std::make_shared<sim::Switch>();
    auto from = receiver;
    auto link = std::make_shared<LinkMock>(from, to);

    receiver->update_routing_table(from, link);

    auto linkToDest = receiver->get_link_to_destination(from);
    EXPECT_EQ(linkToDest->get_to(), to);
}

TEST_F(TestReceiver, UpdateRoutingTableNullptrLink) {
    receiver->update_routing_table(nullptr, nullptr);

    EXPECT_EQ(receiver->get_neighbours().size(), 0);
}

TEST_F(TestReceiver, UpdateRoutingTableNullptrDest) {
    auto to = std::make_shared<sim::Switch>();
    ;
    auto from = receiver;
    auto link = std::make_shared<LinkMock>(from, to);

    receiver->update_routing_table(nullptr, link);

    EXPECT_EQ(receiver->get_neighbours().size(), 0);
}

TEST_F(TestReceiver, UpdateRoutingTableInvalid) {
    auto from = std::make_shared<sim::Switch>();
    auto to = std::make_shared<sim::Receiver>();
    auto link = std::make_shared<LinkMock>(from, to);

    receiver->update_routing_table(from, link);

    EXPECT_EQ(receiver->get_neighbours().size(), 0);
}

TEST_F(TestReceiver, GetNeighboursAndGetLinkToDestinationValid) {
    auto to1 = std::make_shared<sim::Switch>();
    auto link1 = std::make_shared<LinkMock>(receiver, to1);
    receiver->update_routing_table(to1, link1);

    auto to2 = std::make_shared<sim::Switch>();
    auto link2 = std::make_shared<LinkMock>(receiver, to2);

    receiver->update_routing_table(to2, link2);

    auto neighbours = receiver->get_neighbours();
    EXPECT_EQ(neighbours.size(), 2);
    EXPECT_EQ(neighbours[0], to1);
    EXPECT_EQ(neighbours[1], to2);

    auto linkToDest1 = receiver->get_link_to_destination(to1);
    EXPECT_EQ(linkToDest1->get_to(), to1);

    auto linkToDest2 = receiver->get_link_to_destination(to2);
    EXPECT_EQ(linkToDest2->get_to(), to2);
}

TEST_F(TestReceiver, GetLinkToDestinationInvalid) {
    auto linkToDest = receiver->get_link_to_destination(nullptr);
    EXPECT_EQ(linkToDest, nullptr);
}

TEST_F(TestReceiver, ProcessEmpty) { receiver->process(); }

TEST_F(TestReceiver, Process) {
    auto from = std::make_shared<MockSender>();
    auto to = receiver;
    auto inlink = std::make_shared<LinkMock>(from, to);
    auto outlink = std::make_shared<LinkMock>(to, from);

    receiver->add_inlink(inlink);
    receiver->update_routing_table(from, outlink);

    FlowMock* flow = new FlowMock(from, to);

    auto linkToDest = receiver->get_link_to_destination(flow->get_source());

    sim::Packet p(sim::PacketType::DATA, 0, flow);

    inlink->set_ingress_packet(p);

    receiver->process();

    EXPECT_EQ(outlink->get_arrived_packets().size(), 1);
    EXPECT_EQ(outlink->get_arrived_packets()[0].type, sim::PacketType::ACK);
}

}  // namespace test_receiver
