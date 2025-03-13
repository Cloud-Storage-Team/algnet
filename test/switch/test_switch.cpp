#include <gtest/gtest.h>

#include <vector>

#include "../../source/device.hpp"
#include "../../source/link.hpp"
#include "../../source/packet.hpp"
#include "../../source/switch.hpp"
#include "flow_mock.hpp"
#include "link_mock.hpp"
#include "receiver_mock.hpp"

namespace test {

class TestSwitch : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(TestSwitch, test_no_route_to_destination) {
    sim::Switch switch_device;
    std::shared_ptr<LinkMock> link = std::make_shared<LinkMock>();
    switch_device.add_inlink(link);
    bool exception_catched = false;
    try {
        switch_device.process();
    } catch (const std::runtime_error& e) {
        exception_catched = true;
    }
    ASSERT_TRUE(exception_catched);
}

TEST_F(TestSwitch, test_no_senders) {
    sim::Switch switch_device;
    bool exception_catched = false;
    try {
        switch_device.process();
    } catch (const std::runtime_error& e) {
        exception_catched = true;
    }
    ASSERT_TRUE(exception_catched);
}

void test_senders(size_t senders_count) {
    // create devices
    sim::Switch switch_device;
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
        packets[i] = sim::Packet(sim::PacketType::DATA, 0, &flows[i]);
    }

    // create links
    std::vector<std::shared_ptr<LinkMock> > links;
    links.reserve(senders_count);
    for (size_t i = 0; i < senders_count; i++) {
        links.push_back(std::make_shared<LinkMock>());
    }
    std::shared_ptr<LinkMock> switch_reciever_link =
        std::make_shared<LinkMock>();

    // set ingress packets
    for (size_t i = 0; i < senders_count; i++) {
        links[i]->set_ingress_packet(packets[i]);
    }

    // add inlinks to switch device and update its routing table
    for (size_t i = 0; i < senders_count; i++) {
        switch_device.add_inlink(links[i]);
    }
    switch_device.update_routing_table(receiver, switch_reciever_link);

    for (size_t i = 0; i < senders_count; i++) {
        switch_device.process();
    }

    ASSERT_TRUE(switch_reciever_link->get_arrived_packets() == packets);
}

TEST_F(TestSwitch, test_one_sender) { test_senders(1); }

TEST_F(TestSwitch, test_multiple_senders) { test_senders(5); }

}  // namespace test
