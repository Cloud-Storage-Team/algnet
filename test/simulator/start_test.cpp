#include <gtest/gtest.h>

#include "simulator.hpp"
#include "utils.hpp"

namespace test {

class Start : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST_F(Start, TrivialTopology) {
    sim::Simulator sim;
    auto sender = sim.add_sender("sender");
    auto swtch = sim.add_switch("switch");
    auto receiver = sim.add_receiver("receiver");

    constexpr Time delay_between_packets = 100;
    constexpr Time stop_time = 1000;
    constexpr Size packet_size = 1024;

    std::shared_ptr<sim::Flow> flow = std::make_shared<sim::Flow>(
        sender, receiver, packet_size, delay_between_packets);

    sim.add_flow(flow);

    add_two_way_links(sim, {{sender, swtch}, {swtch, receiver}});

    sim.start(stop_time);

    ASSERT_EQ(flow->get_updated(), true);
}

}  // namespace test
