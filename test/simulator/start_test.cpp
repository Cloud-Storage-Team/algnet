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
    sim::Simulator sim;
    auto sender = sim.add_device("sender", sim::DeviceType::SENDER);
    auto swtch = sim.add_device("switch", sim::DeviceType::SWITCH);
    auto receiver = sim.add_device("receiver", sim::DeviceType::RECEIVER);

    constexpr Time delay_between_packets = 100;
    constexpr Time stop_time = 1000;
    constexpr Size packet_size = 1024;

    sim.add_flow(sender, receiver, delay_between_packets, packet_size);

    add_two_way_links(sim, {{sender, swtch}, {swtch, receiver}});

    sim.start(stop_time);
    
    // Test something...
}

}  // namespace test
