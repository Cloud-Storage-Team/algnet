#include <gtest/gtest.h>

#include <vector>

#include "../../source/device.hpp"
#include "../../source/link.hpp"
#include "../../source/switch.hpp"
#include "../../source/packet.hpp"
#include "device_mock.hpp"
#include "link_mock.hpp"

namespace {

class TestSwitch : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

void test_senders(size_t senders_count) {
    // create devices
    std::vector<DeviceMock> senders(senders_count,
                                    DeviceMock(sim::DeviceType::SENDER));
    sim::Switch switch_device;
    DeviceMock receiver(sim::DeviceType::RECEIVER);

    // create flows
    std::vector<sim::Flow> flows;
    flows.reserve(senders_count);
    for (size_t i = 0; i < senders_count; i++) {
        flows.push_back(sim::Flow(&senders[i], &receiver, 0));
    }

    // create packets
    std::vector<sim::Packet> packets(senders_count);
    for (size_t i = 0; i < senders_count; i++) {
        packets[i] = sim::Packet(sim::PacketType::DATA, 0, &flows[i]);
    }

    // create links
    std::vector<LinkMock> links;
    links.reserve(senders_count);
    for (size_t i = 0; i < senders_count; i++) {
        links.push_back(LinkMock(&senders[i], &switch_device));
    }
    LinkMock switch_reciever_link(&switch_device, &receiver);

    // set ingress packets
    for (size_t i = 0; i < senders_count; i++) {
        links[i].set_ingress_packet(packets[i]);
    }

    // add inlinks to switch device and update its routing table
    for (size_t i = 0; i < senders_count; i++) {
        switch_device.add_inlink(&links[i]);
    }
    switch_device.update_routing_table(&receiver, &switch_reciever_link);

    for (size_t i = 0; i < senders_count; i++) {
        switch_device.process();
    }

    ASSERT_TRUE(switch_reciever_link.get_arrived_packets() == packets);
}

TEST_F(TestSwitch, test_one_sender) {
    test_senders(1);
}

TEST_F(TestSwitch, test_multiple_senders) {
    test_senders(5);
}

}
