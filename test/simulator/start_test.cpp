#include <gtest/gtest.h>

#include "simulator.hpp"
#include "utils.hpp"

#include "spdlog/fmt/bundled/format.h"

namespace test {

class Start : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};

    Id get_flow_name() {
        static int counter = 1;
        return fmt::format("flow_{}", counter++);
    }
};

TEST_F(Start, TrivialTopology) {
    TestSimulator sim;
    Id sender_id = sim.add_sender("sender");
    Id switch_id = sim.add_switch("switch");
    Id receiver_id = sim.add_receiver("receiver");

    auto sender = sim.get_sender(sender_id);
    auto swtch = sim.get_switch(switch_id);
    auto receiver = sim.get_receiver(receiver_id);

    constexpr Time delay_between_packets = 100;
    constexpr Time stop_time = 1000;
    constexpr Size packet_size = 1024;
    constexpr std::uint32_t packets_to_send = 1;

    auto flow_id = sim.add_flow(get_flow_name(), "sender", "receiver", packet_size,
                                delay_between_packets, packets_to_send);

    add_two_way_links(sim, {{"sender", "switch"}, {"switch", "receiver"}});

    sim.start(stop_time);

    ASSERT_EQ(sim.get_flow(flow_id)->get_updates_number(), packets_to_send);
}

TEST_F(Start, ThreeToOneTopology) {
    TestSimulator sim;
    Id sender1_id = sim.add_sender("sender1");
    Id sender2_id = sim.add_sender("sender2");
    Id sender3_id = sim.add_sender("sender3");
    Id switch_id = sim.add_switch("switch");
    Id receiver_id = sim.add_receiver("receiver");

    auto sender1 = sim.get_sender(sender1_id);
    auto sender2 = sim.get_sender(sender2_id);
    auto sender3 = sim.get_sender(sender3_id);
    auto swtch = sim.get_switch(switch_id);
    auto receiver = sim.get_receiver(receiver_id);

    add_two_way_links(sim, {{"sender1", "switch"},
                            {"sender2", "switch"},
                            {"sender3", "switch"},
                            {"switch", "receiver"}});

    constexpr Time delay_between_packets = 100;
    constexpr Time stop_time = 10000;
    constexpr Size packet_size = 1024;
    constexpr std::uint32_t packets_to_send_by_flow1 = 10;
    constexpr std::uint32_t packets_to_send_by_flow2 = 50;
    constexpr std::uint32_t packets_to_send_by_flow3 = 100;

    auto flow1_id =
        sim.add_flow(get_flow_name(),"sender1", "receiver", packet_size, delay_between_packets,
                     packets_to_send_by_flow1);
    auto flow2_id =
        sim.add_flow(get_flow_name(),"sender2", "receiver", packet_size, delay_between_packets,
                     packets_to_send_by_flow2);
    auto flow3_id =
        sim.add_flow(get_flow_name(),"sender3", "receiver", packet_size, delay_between_packets,
                     packets_to_send_by_flow3);

    sim.start(stop_time);

    ASSERT_EQ(sim.get_flow(flow1_id)->get_updates_number(),
              packets_to_send_by_flow1);
    ASSERT_EQ(sim.get_flow(flow2_id)->get_updates_number(),
              packets_to_send_by_flow2);
    ASSERT_EQ(sim.get_flow(flow3_id)->get_updates_number(),
              packets_to_send_by_flow3);
}

TEST_F(Start, StopTime) {
    TestSimulator sim;
    Id sender1_id = sim.add_sender("sender1");
    Id sender2_id = sim.add_sender("sender2");
    Id sender3_id = sim.add_sender("sender3");
    Id switch_id = sim.add_switch("switch");
    Id receiver_id = sim.add_receiver("receiver");

    auto sender1 = sim.get_sender(sender1_id);
    auto sender2 = sim.get_sender(sender2_id);
    auto sender3 = sim.get_sender(sender3_id);
    auto swtch = sim.get_switch(switch_id);
    auto receiver = sim.get_receiver(receiver_id);

    add_two_way_links(sim, {{"sender1", "switch"},
                            {"sender2", "switch"},
                            {"sender3", "switch"},
                            {"switch", "receiver"}});

    constexpr Time delay_between_packets = 100;
    constexpr Time stop_time = 1000;
    constexpr Size packet_size = 1024;
    constexpr std::uint32_t packets_to_send_by_flow1 = 10;
    constexpr std::uint32_t packets_to_send_by_flow2 = 50;
    constexpr std::uint32_t packets_to_send_by_flow3 = 100;

    auto flow1_id =
        sim.add_flow(get_flow_name(),"sender1", "receiver", packet_size, delay_between_packets,
                     packets_to_send_by_flow1);
    auto flow2_id =
        sim.add_flow(get_flow_name(),"sender2", "receiver", packet_size, delay_between_packets,
                     packets_to_send_by_flow2);
    auto flow3_id =
        sim.add_flow(get_flow_name(),"sender3", "receiver", packet_size, delay_between_packets,
                     packets_to_send_by_flow3);

    sim.start(stop_time);

    // First flow generates all packets in time
    ASSERT_TRUE(sim.get_flow(flow1_id)->get_updates_number() ==
                packets_to_send_by_flow1);

    // Second and third flows have no time to generate all packets (stop_time <
    // packets_to_send * generate_delay)
    ASSERT_TRUE(sim.get_flow(flow2_id)->get_updates_number() <
                packets_to_send_by_flow2);
    ASSERT_TRUE(sim.get_flow(flow3_id)->get_updates_number() <
                packets_to_send_by_flow3);
}

}  // namespace test
