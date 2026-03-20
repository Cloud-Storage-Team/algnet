#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <vector>

#include "../utils/fake_packet.hpp"
#include "host_mock.hpp"
#include "link_mock.hpp"
#include "network/connection/flow/packet.hpp"
#include "scheduler/scheduler.hpp"
#include "topology/device/host.hpp"
#include "topology/device/switch.hpp"

namespace test {

class TestSwitch : public testing::Test {
public:
    void TearDown() override { sim::Scheduler::get_instance().clear(); };
    void SetUp() override { sim::Scheduler::get_instance().clear(); };
};

TEST_F(TestSwitch, test_add_nullptr_link) {
    auto switch_device = std::make_shared<sim::Switch>("");
    ASSERT_FALSE(switch_device->add_inlink(nullptr));
}

TEST_F(TestSwitch, test_add_incorrect_inlink) {
    auto switch_device = std::make_shared<sim::Switch>("");
    std::shared_ptr<sim::IDevice> null_device(nullptr);
    std::shared_ptr<LinkMock> link =
        std::make_shared<LinkMock>(null_device, null_device);
    ASSERT_FALSE(switch_device->add_inlink(link));
}

// Real system does not know if another device exists or not, so this test is
// not correct at all TEST_F(TestSwitch, nullptr_destination_device) {
//     auto switch_device = std::make_shared<sim::Switch>();
//     auto temp_device = std::make_shared<test::HostMock>();
//     auto link = std::make_shared<LinkMock>(switch_device, temp_device);
//     ASSERT_FALSE(switch_device->update_routing_table(nullptr, link));
// }

TEST_F(TestSwitch, nullptr_outlink) {
    auto switch_device = std::make_shared<sim::Switch>("");
    auto temp_device = std::make_shared<test::HostMock>();
    ASSERT_FALSE(
        switch_device->update_routing_table(temp_device->get_id(), nullptr));
}

TEST_F(TestSwitch, add_foreign_inlink) {
    auto switch_device = std::make_shared<sim::Switch>("test");
    auto temp_device = std::make_shared<test::HostMock>();
    auto link = std::make_shared<LinkMock>(temp_device, switch_device);
    ASSERT_FALSE(
        switch_device->update_routing_table(temp_device->get_id(), link));
}

TEST_F(TestSwitch, test_no_destination_route) {
    auto switch_device = std::make_shared<sim::Switch>("");
    auto receiver = std::make_shared<HostMock>();
    sim::Packet packet(SizeByte(0));

    std::shared_ptr<sim::IDevice> null_device(nullptr);
    std::shared_ptr<LinkMock> switch_inlink =
        std::make_shared<LinkMock>(null_device, switch_device);
    switch_device->add_inlink(switch_inlink);
    switch_inlink->set_ingress_packet(packet);

    std::shared_ptr<LinkMock> switch_reciever_link =
        std::make_shared<LinkMock>(switch_device, receiver);
    // no update of switch routing table

    switch_device->notify_about_arrival();

    while (sim::Scheduler::get_instance().tick()) {
    }

    ASSERT_EQ(switch_reciever_link->get_arrived_packets(),
              std::vector<sim::Packet>());
}

// this test checks that packets passed throw same path have equal path hash
// and packets passed throw different pathes have different path hashes
TEST_F(TestSwitch, test_path_hash) {
    // topology:
    // sender  --- switch_1
    //   |            |
    //   |            |
    // switch_2 -- receiver
    auto sender = std::make_shared<sim::Host>("sender");
    auto switch_1 = std::make_shared<sim::Switch>("switch_1");
    auto switch_2 = std::make_shared<sim::Switch>("switch_2");
    auto receiver = std::make_shared<sim::Host>("receiver");

    auto link_sender_to_switch_1 = std::make_shared<LinkMock>(sender, switch_1);
    switch_1->add_inlink(link_sender_to_switch_1);

    auto link_sender_to_switch_2 = std::make_shared<LinkMock>(sender, switch_2);
    switch_2->add_inlink(link_sender_to_switch_2);

    auto link_switch_1_to_receiver =
        std::make_shared<LinkMock>(switch_1, receiver);
    switch_1->update_routing_table(receiver->get_id(),
                                   link_switch_1_to_receiver);

    auto link_switch_2_to_receiver =
        std::make_shared<LinkMock>(switch_2, receiver);
    switch_2->update_routing_table(receiver->get_id(),
                                   link_switch_2_to_receiver);

    sim::Packet packet_template(SizeByte(1), sender->get_id(),
                                receiver->get_id());
    sim::Packet first_packet_route_1(packet_template);
    sim::Packet second_packet_route_1(packet_template);
    sim::Packet packet_route_2(packet_template);

    link_sender_to_switch_1->set_ingress_packet(first_packet_route_1);
    switch_1->notify_about_arrival();
    while (sim::Scheduler::get_instance().tick()) {
    }

    link_sender_to_switch_1->set_ingress_packet(second_packet_route_1);
    switch_1->notify_about_arrival();
    while (sim::Scheduler::get_instance().tick()) {
    }

    auto arrived_packets_route_1 =
        link_switch_1_to_receiver->get_arrived_packets();
    ASSERT_EQ(arrived_packets_route_1.size(), 2);

    ASSERT_EQ(arrived_packets_route_1[0].path_hash,
              arrived_packets_route_1[1].path_hash);

    link_sender_to_switch_2->set_ingress_packet(packet_route_2);
    switch_2->notify_about_arrival();
    while (sim::Scheduler::get_instance().tick()) {
    }

    auto arrived_packets_route_2 =
        link_switch_2_to_receiver->get_arrived_packets();
    ASSERT_EQ(arrived_packets_route_2.size(), 1);
    ASSERT_NE(arrived_packets_route_2[0].path_hash,
              arrived_packets_route_1[0].path_hash);
}

}  // namespace test
