#include <gtest/gtest.h>

#include <vector>

#include "device.hpp"
#include "flow_mock.hpp"
#include "link.hpp"
#include "link_mock.hpp"
#include "packet.hpp"
#include "receiver_mock.hpp"
#include "switch.hpp"

namespace test {

class TestSwitch : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(TestSwitch, test_add_nullptr_link) {
    sim::Switch switch_device;
    ASSERT_FALSE(switch_device.add_inlink(nullptr));
}

TEST_F(TestSwitch, test_add_incorrect_inlink) {
    sim::Switch switch_device;
    std::shared_ptr<sim::IRoutingDevice> null_device(nullptr);
    std::shared_ptr<LinkMock> link =
        std::make_shared<LinkMock>(null_device, null_device);
    ASSERT_FALSE(switch_device.add_inlink(link));
}

TEST_F(TestSwitch, nullptr_destination_device) {
    auto switch_device = std::make_shared<sim::Switch>();
    auto temp_device = std::make_shared<test::ReceiverMock>();
    auto link = std::make_shared<LinkMock>(switch_device, temp_device);
    ASSERT_FALSE(switch_device->update_routing_table(nullptr, link));
}

TEST_F(TestSwitch, nullptr_outlink) {
    auto switch_device = std::make_shared<sim::Switch>();
    auto temp_device = std::make_shared<test::ReceiverMock>();
    ASSERT_FALSE(switch_device->update_routing_table(temp_device, nullptr));
}

TEST_F(TestSwitch, add_foreign_inlink) {
    auto switch_device = std::make_shared<sim::Switch>();
    auto temp_device = std::make_shared<test::ReceiverMock>();
    auto link = std::make_shared<LinkMock>(temp_device, switch_device);
    ASSERT_FALSE(switch_device->update_routing_table(temp_device, link));
}

TEST_F(TestSwitch, test_no_senders) {
    sim::Switch switch_device;
    // TODO: replace with ASSERT_FALSE when process returns bool instead of
    // void
    switch_device.process();
}

TEST_F(TestSwitch, test_no_packets_on_inlinks) {
    auto switch_device = std::make_shared<sim::Switch>();

    // create links
    std::shared_ptr<sim::IRoutingDevice> null_device(nullptr);
    std::shared_ptr<LinkMock> switch_inlink =
        std::make_shared<LinkMock>(null_device, switch_device);

    switch_device->add_inlink(switch_inlink);

    switch_device->process();
    // TODO: add ASSERT_FALSE when process returns bool instead of void
}

TEST_F(TestSwitch, test_no_destination_route) {
    auto switch_device = std::make_shared<sim::Switch>();
    auto receiver = std::make_shared<ReceiverMock>();
    FlowMock flow(receiver);
    sim::Packet packet(sim::PacketType::DATA, 0, &flow);

    std::shared_ptr<sim::IRoutingDevice> null_device(nullptr);
    std::shared_ptr<LinkMock> switch_inlink =
        std::make_shared<LinkMock>(null_device, switch_device);
    switch_device->add_inlink(switch_inlink);
    switch_inlink->set_ingress_packet(packet);

    std::shared_ptr<LinkMock> switch_reciever_link =
        std::make_shared<LinkMock>(switch_device, receiver);
    // no update of switch routing table

    // TODO: add ASSERT_FALSE when process returns bool instead of void
    switch_device->process();

    ASSERT_EQ(switch_reciever_link->get_arrived_packets(),
              std::vector<sim::Packet>());
}

static bool compare_packets(const sim::Packet& p1, const sim::Packet& p2) {
    return p1.flow < p2.flow;
}

void test_senders(size_t senders_count) {
    // create devices
    auto switch_device = std::make_shared<sim::Switch>();
    std::shared_ptr<ReceiverMock> receiver = std::make_shared<ReceiverMock>();
    // create flows
    std::vector<FlowMock> flows;
    flows.reserve(senders_count);
    for (size_t i = 0; i < senders_count; i++) {
        flows.push_back(FlowMock(receiver));
    }

    // create packets
    std::vector<sim::Packet> packets(senders_count);
    for (size_t i = 0; i < senders_count; i++) {
        packets[i] = sim::Packet(sim::PacketType::DATA, i, &flows[i]);
    }

    // create links
    std::vector<std::shared_ptr<LinkMock> > links;
    links.reserve(senders_count);

    std::shared_ptr<sim::IRoutingDevice> null_device(nullptr);
    for (size_t i = 0; i < senders_count; i++) {
        links.push_back(std::make_shared<LinkMock>(null_device, switch_device));
    }
    std::shared_ptr<LinkMock> switch_reciever_link =
        std::make_shared<LinkMock>(switch_device, receiver);
    switch_device->update_routing_table(receiver, switch_reciever_link);

    // set ingress packets
    for (size_t i = 0; i < senders_count; i++) {
        links[i]->set_ingress_packet(packets[i]);
    }

    // add inlinks to switch device and update its routing table
    for (size_t i = 0; i < senders_count; i++) {
        switch_device->add_inlink(links[i]);
    }

    for (size_t i = 0; i < senders_count; i++) {
        switch_device->process();
    }
    std::vector<sim::Packet> arrived_packets =
        switch_reciever_link->get_arrived_packets();

    std::sort(packets.begin(), packets.end(), compare_packets);
    std::sort(arrived_packets.begin(), arrived_packets.end(), compare_packets);
    ASSERT_EQ(arrived_packets, packets);
}

TEST_F(TestSwitch, test_one_sender) { test_senders(1); }

TEST_F(TestSwitch, test_multiple_senders) { test_senders(5); }

}  // namespace test
