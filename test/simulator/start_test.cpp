#include <gtest/gtest.h>

#include "simulator.hpp"
#include "flow/tcp/basic/basic_cc.hpp"
#include "utils.hpp"

namespace test {

class Start : public testing::Test {
public:
    void TearDown() override {}
    void SetUp() override {};
};

TEST_F(Start, TrivialTopology) {
    sim::Simulator sim;
    auto sender = std::make_shared<sim::Host>("sender");
    sim.add_host(sender);

    auto swtch = std::make_shared<sim::Switch>("switch");
    sim.add_switch(swtch);

    auto receiver = std::make_shared<sim::Host>("receiver");
    sim.add_host(receiver);

    constexpr TimeNs stop_time(1000);
    constexpr SizeByte packet_size(1024);
    constexpr std::uint32_t packets_to_send = 1;

    Id id = "flow";
    auto flow = std::make_shared<sim::TcpFlow>(
        id, sender, receiver, std::make_unique<sim::BasicCC>(), packet_size, packets_to_send);
    sim.add_flow(flow);

    add_two_way_links(sim, {{sender, swtch}, {swtch, receiver}});

    sim.start(stop_time);

    ASSERT_EQ(flow->get_delivered_bytes(), packet_size * packets_to_send);
}

TEST_F(Start, ThreeToOneTopology) {
    sim::Simulator sim;
    auto sender1 = std::make_shared<sim::Host>("sender1");
    sim.add_host(sender1);

    auto sender2 = std::make_shared<sim::Host>("sender2");
    sim.add_host(sender2);

    auto sender3 = std::make_shared<sim::Host>("sender3");
    sim.add_host(sender3);

    auto swtch = std::make_shared<sim::Switch>("switch");
    sim.add_switch(swtch);

    auto receiver = std::make_shared<sim::Host>("receiver");
    sim.add_host(receiver);

    add_two_way_links(sim, {{sender1, swtch},
                            {sender2, swtch},
                            {sender3, swtch},
                            {swtch, receiver}});

    constexpr TimeNs stop_time(10000);
    constexpr SizeByte packet_size(10);
    constexpr std::uint32_t packets_to_send_by_flow1 = 10;
    constexpr std::uint32_t packets_to_send_by_flow2 = 50;
    constexpr std::uint32_t packets_to_send_by_flow3 = 100;

    Id id_1 = "flow_1";
    auto flow1 = std::make_shared<sim::TcpFlow>(id_1, sender1, receiver,
                                                  std::make_unique<sim::BasicCC>(), packet_size,
                                                  packets_to_send_by_flow1);
    sim.add_flow(flow1);

    Id id_2 = "flow_2";
    auto flow2 = std::make_shared<sim::TcpFlow>(id_2, sender2, receiver,
                                                  std::make_unique<sim::BasicCC>(), packet_size,
                                                  packets_to_send_by_flow2);
    sim.add_flow(flow2);

    Id id_3 = "flow_3";
    auto flow3 = std::make_shared<sim::TcpFlow>(id_3, sender3, receiver,
                                                  std::make_unique<sim::BasicCC>(), packet_size,
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
