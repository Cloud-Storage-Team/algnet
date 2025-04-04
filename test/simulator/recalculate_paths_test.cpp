#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "device.hpp"
#include "simulator.hpp"

namespace test {

class RecalculatePaths : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

constexpr std::uint32_t speed_stub = 0;
constexpr std::uint32_t delay_stub = 0;

/*
          sender
            |
            1
            |
          switch
            |
            2
            |
         receiver
*/

TEST_F(RecalculatePaths, TrivialTopology) {
    sim::Simulator sim;

    auto sender = sim.add_device("sender", sim::DeviceType::SENDER);
    auto swtch = sim.add_device("switch", sim::DeviceType::SWITCH);
    auto receiver = sim.add_device("receiver", sim::DeviceType::RECEIVER);

    sim.add_link(sender, swtch, speed_stub, delay_stub);
    sim.add_link(swtch, sender, speed_stub, delay_stub);

    sim.add_link(swtch, receiver, speed_stub, delay_stub);
    sim.add_link(receiver, swtch, speed_stub, delay_stub);

    sim.recalculate_paths();

    EXPECT_NE(sender->get_link_to_destination(receiver), nullptr);
    EXPECT_NE(swtch->get_link_to_destination(sender), nullptr);
    EXPECT_NE(swtch->get_link_to_destination(receiver), nullptr);
    EXPECT_NE(receiver->get_link_to_destination(swtch), nullptr);

    EXPECT_TRUE(sender->get_link_to_destination(receiver)->get_to() == swtch);
    EXPECT_TRUE(sender->get_link_to_destination(swtch)->get_to() == swtch);

    EXPECT_TRUE(swtch->get_link_to_destination(sender)->get_to() == sender);
    EXPECT_TRUE(swtch->get_link_to_destination(receiver)->get_to() == receiver);

    EXPECT_TRUE(receiver->get_link_to_destination(sender)->get_to() == swtch);
    EXPECT_TRUE(receiver->get_link_to_destination(swtch)->get_to() == swtch);
}

/*
    sender1    sender2
         \      /
         1     2
          \   /
          switch
            |
            3
            |
         receiver
*/

TEST_F(RecalculatePaths, SimpleTopology) {
    sim::Simulator sim;

    auto sender1 = sim.add_device("sender1", sim::DeviceType::SENDER);
    auto sender2 = sim.add_device("sender2", sim::DeviceType::SENDER);
    auto swtch = sim.add_device("switch", sim::DeviceType::SWITCH);
    auto receiver = sim.add_device("receiver", sim::DeviceType::RECEIVER);
    
    sim.add_link(sender1, swtch, speed_stub, delay_stub);
    sim.add_link(swtch, sender1, speed_stub, delay_stub);

    sim.add_link(sender2, swtch, speed_stub, delay_stub);
    sim.add_link(swtch, sender2, speed_stub, delay_stub);
    
    sim.add_link(swtch, receiver, speed_stub, delay_stub);
    sim.add_link(receiver, swtch, speed_stub, delay_stub);

    sim.recalculate_paths();

    spdlog::info("Recalculated paths");

    EXPECT_NE(sender1->get_link_to_destination(receiver), nullptr);
    EXPECT_NE(sender1->get_link_to_destination(sender2), nullptr);
    EXPECT_NE(sender1->get_link_to_destination(swtch), nullptr);

    EXPECT_TRUE(sender1->get_link_to_destination(receiver)->get_to() == swtch);
    EXPECT_TRUE(sender1->get_link_to_destination(swtch)->get_to() == swtch);

    EXPECT_TRUE(sender2->get_link_to_destination(receiver)->get_to() == swtch);
    EXPECT_TRUE(sender2->get_link_to_destination(swtch)->get_to() == swtch);

    EXPECT_TRUE(swtch->get_link_to_destination(sender2)->get_to() == sender2);
    EXPECT_TRUE(swtch->get_link_to_destination(sender1)->get_to() == sender1);
    EXPECT_TRUE(swtch->get_link_to_destination(receiver)->get_to() == receiver);

    EXPECT_TRUE(receiver->get_link_to_destination(sender1)->get_to() == swtch);
    EXPECT_TRUE(receiver->get_link_to_destination(sender2)->get_to() == swtch);
    EXPECT_TRUE(receiver->get_link_to_destination(swtch)->get_to() == swtch);
}

}  // namespace test
