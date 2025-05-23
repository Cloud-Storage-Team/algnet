#include <gtest/gtest.h>

#include "simulator.hpp"
#include "utils.hpp"

namespace test {

class Start : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(Start, TrivialTopology) {
    sim::BasicSimulator sim;
    auto sender = sim.add_sender("sender");
    auto swtch = sim.add_switch("switch");
    auto receiver = sim.add_receiver("receiver");

    constexpr Time delay_between_packets = 100;
    constexpr Time stop_time = 1000;
    constexpr Size packet_size = 1024;
    constexpr std::uint32_t packets_to_send = 1;

    auto flow = sim.add_flow(sender, receiver, packet_size,
                             delay_between_packets, packets_to_send);

    add_two_way_links(sim, {{sender, swtch}, {swtch, receiver}});

    sim.start(stop_time);

    ASSERT_EQ(flow->get_updates_number(), packets_to_send);
}

TEST_F(Start, ThreeToOneTopology) {
    sim::BasicSimulator sim;
    auto sender1 = sim.add_sender("sender1");
    auto sender2 = sim.add_sender("sender2");
    auto sender3 = sim.add_sender("sender3");
    auto swtch = sim.add_switch("switch");
    auto receiver = sim.add_receiver("receiver");

    add_two_way_links(sim, {{sender1, swtch},
                            {sender2, swtch},
                            {sender3, swtch},
                            {swtch, receiver}});

    constexpr Time delay_between_packets = 100;
    constexpr Time stop_time = 10000;
    constexpr Size packet_size = 1024;
    constexpr std::uint32_t packets_to_send_by_flow1 = 10;
    constexpr std::uint32_t packets_to_send_by_flow2 = 50;
    constexpr std::uint32_t packets_to_send_by_flow3 = 100;

    auto flow1 = sim.add_flow(sender1, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow1);
    auto flow2 = sim.add_flow(sender2, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow2);
    auto flow3 = sim.add_flow(sender3, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow3);

    sim.start(stop_time);

    ASSERT_EQ(flow1->get_updates_number(), packets_to_send_by_flow1);
    ASSERT_EQ(flow2->get_updates_number(), packets_to_send_by_flow2);
    ASSERT_EQ(flow3->get_updates_number(), packets_to_send_by_flow3);
}

TEST_F(Start, StopTime) {
    sim::BasicSimulator sim;
    auto sender1 = sim.add_sender("sender1");
    auto sender2 = sim.add_sender("sender2");
    auto sender3 = sim.add_sender("sender3");
    auto swtch = sim.add_switch("switch");
    auto receiver = sim.add_receiver("receiver");

    add_two_way_links(sim, {{sender1, swtch},
                            {sender2, swtch},
                            {sender3, swtch},
                            {swtch, receiver}});

    constexpr Time delay_between_packets = 100;
    constexpr Time stop_time = 1000;
    constexpr Size packet_size = 1024;
    constexpr std::uint32_t packets_to_send_by_flow1 = 10;
    constexpr std::uint32_t packets_to_send_by_flow2 = 50;
    constexpr std::uint32_t packets_to_send_by_flow3 = 100;

    auto flow1 = sim.add_flow(sender1, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow1);
    auto flow2 = sim.add_flow(sender2, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow2);
    auto flow3 = sim.add_flow(sender3, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow3);

    sim.start(stop_time);

    // First flow generates all packets in time
    ASSERT_TRUE(flow1->get_updates_number() == packets_to_send_by_flow1);

    // Second and third flows have no time to generate all packets (stop_time <
    // packets_to_send * generate_delay)
    ASSERT_TRUE(flow2->get_updates_number() < packets_to_send_by_flow2);
    ASSERT_TRUE(flow3->get_updates_number() < packets_to_send_by_flow3);
}

}  // namespace test
