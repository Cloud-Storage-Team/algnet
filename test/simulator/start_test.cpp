#include <gtest/gtest.h>

#include "simulator.hpp"
#include "utils.hpp"

namespace test {

class Start : public testing::Test {
public:
    void TearDown() override { sim::IdentifierFactory::get_instance().clear(); }
    void SetUp() override { sim::IdentifierFactory::get_instance().clear(); };
};

TEST_F(Start, TrivialTopology) {
    sim::BasicSimulator sim;
    Id sender_id = "sender";
    auto sender =
        sim::IdentifierFactory::get_instance().create_object<sim::Host>(
            sender_id);
    sim.add_host(sender);

    Id switch_id = "switch";
    auto swtch = std::make_shared<sim::Switch>(switch_id);
    sim.add_switch(swtch);

    Id receiver_id = "receiver";
    auto receiver =
        sim::IdentifierFactory::get_instance().create_object<sim::Host>(
            receiver_id);
    sim.add_host(receiver);

    constexpr TimeNs stop_time(1000);
    constexpr SizeByte packet_size(1024);
    constexpr std::uint32_t packets_to_send = 1;

    Id id = "flow";
    auto flow = std::make_shared<sim::BasicFlow>(
        id, sim::Route(sender_id, receiver_id), sim::BasicCC(), packet_size,
        packets_to_send);
    sim.add_flow(flow);

    add_two_way_links(sim, {{sender, swtch}, {swtch, receiver}});

    sim.start(stop_time);

    ASSERT_EQ(flow->get_delivered_bytes(), packet_size * packets_to_send);
}

TEST_F(Start, ThreeToOneTopology) {
    sim::BasicSimulator sim;
    Id sender_1_id = "sender-1";
    auto sender_1 =
        sim::IdentifierFactory::get_instance().create_object<sim::Host>(
            sender_1_id);
    sim.add_host(sender_1);

    Id sender_2_id = "sender-2";
    auto sender_2 =
        sim::IdentifierFactory::get_instance().create_object<sim::Host>(
            sender_2_id);
    sim.add_host(sender_2);

    Id sender_3_id = "sender-3";
    auto sender_3 =
        sim::IdentifierFactory::get_instance().create_object<sim::Host>(
            sender_3_id);
    sim.add_host(sender_3);

    Id switch_id = "switch";
    auto swtch = std::make_shared<sim::Switch>(switch_id);
    sim.add_switch(swtch);

    Id receiver_id = "receiver";
    auto receiver =
        sim::IdentifierFactory::get_instance().create_object<sim::Host>(
            receiver_id);
    sim.add_host(receiver);

    add_two_way_links(sim, {{sender_1, swtch},
                            {sender_2, swtch},
                            {sender_3, swtch},
                            {swtch, receiver}});

    constexpr TimeNs stop_time(10000);
    constexpr SizeByte packet_size(10);
    constexpr std::uint32_t packets_to_send_by_flow1 = 10;
    constexpr std::uint32_t packets_to_send_by_flow2 = 50;
    constexpr std::uint32_t packets_to_send_by_flow3 = 100;

    Id id_1 = "flow_1";
    auto flow1 = std::make_shared<sim::BasicFlow>(
        id_1, sim::Route(sender_1_id, receiver_id), sim::BasicCC(), packet_size,
        packets_to_send_by_flow1);
    sim.add_flow(flow1);

    Id id_2 = "flow_2";
    auto flow2 = std::make_shared<sim::BasicFlow>(
        id_2, sim::Route(sender_2_id, receiver_id), sim::BasicCC(), packet_size,
        packets_to_send_by_flow2);
    sim.add_flow(flow2);

    Id id_3 = "flow_3";
    auto flow3 = std::make_shared<sim::BasicFlow>(
        id_3, sim::Route(sender_3_id, receiver_id), sim::BasicCC(), packet_size,
        packets_to_send_by_flow3);
    sim.add_flow(flow3);

    sim.start(stop_time);

    ASSERT_EQ(flow1->get_delivered_bytes(),
              packet_size * packets_to_send_by_flow1);
    ASSERT_EQ(flow2->get_delivered_bytes(),
              packet_size * packets_to_send_by_flow2);
    ASSERT_EQ(flow3->get_delivered_bytes(),
              packet_size * packets_to_send_by_flow3);
}

}  // namespace test
