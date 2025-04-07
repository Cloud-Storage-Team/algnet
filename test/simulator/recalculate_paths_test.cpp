#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "device.hpp"
#include "simulator.hpp"
#include "utils.hpp"

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

    add_links(sim, {{sender, swtch}, {swtch, receiver}});

    sim.recalculate_paths();

    EXPECT_NE(sender->get_link_to_destination(receiver), nullptr);
    EXPECT_NE(swtch->get_link_to_destination(sender), nullptr);
    EXPECT_NE(swtch->get_link_to_destination(receiver), nullptr);
    EXPECT_NE(receiver->get_link_to_destination(swtch), nullptr);

    EXPECT_EQ(sender->get_link_to_destination(receiver)->get_to(), swtch);
    EXPECT_EQ(sender->get_link_to_destination(swtch)->get_to(), swtch);

    EXPECT_EQ(swtch->get_link_to_destination(sender)->get_to(), sender);
    EXPECT_EQ(swtch->get_link_to_destination(receiver)->get_to(), receiver);

    EXPECT_EQ(receiver->get_link_to_destination(sender)->get_to(), swtch);
    EXPECT_EQ(receiver->get_link_to_destination(swtch)->get_to(), swtch);
}

/*
    sender1    sender2
        \       /
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

    add_links(sim, {{sender1, swtch}, {sender2, swtch}, {swtch, receiver}});

    sim.recalculate_paths();

    EXPECT_NE(sender1->get_link_to_destination(receiver), nullptr);
    EXPECT_NE(sender1->get_link_to_destination(sender2), nullptr);
    EXPECT_NE(sender1->get_link_to_destination(swtch), nullptr);

    EXPECT_EQ(sender1->get_link_to_destination(receiver)->get_to(), swtch);
    EXPECT_EQ(sender1->get_link_to_destination(swtch)->get_to(), swtch);

    EXPECT_EQ(sender2->get_link_to_destination(receiver)->get_to(), swtch);
    EXPECT_EQ(sender2->get_link_to_destination(swtch)->get_to(), swtch);

    EXPECT_EQ(swtch->get_link_to_destination(sender2)->get_to(), sender2);
    EXPECT_EQ(swtch->get_link_to_destination(sender1)->get_to(), sender1);
    EXPECT_EQ(swtch->get_link_to_destination(receiver)->get_to(), receiver);

    EXPECT_EQ(receiver->get_link_to_destination(sender1)->get_to(), swtch);
    EXPECT_EQ(receiver->get_link_to_destination(sender2)->get_to(), swtch);
    EXPECT_EQ(receiver->get_link_to_destination(swtch)->get_to(), swtch);
}

/*
    sender1    sender2    sender3
         \      /           /
         1     2           /
          \   /           /
          switch1        3
         /    |  \      /
        /     |   6    /
       /      |    \  /
      4       5   switch2
     /        |     /  \
    /         |    7    8
   /          |   /      \
receiver1 receiver2  receiver3
*/

TEST_F(RecalculatePaths, MeshTopology) {
    sim::Simulator sim;

    auto sender1 = sim.add_device("sender1", sim::DeviceType::SENDER);
    auto sender2 = sim.add_device("sender2", sim::DeviceType::SENDER);
    auto sender3 = sim.add_device("sender3", sim::DeviceType::SENDER);
    auto swtch1 = sim.add_device("switch1", sim::DeviceType::SWITCH);
    auto swtch2 = sim.add_device("switch2", sim::DeviceType::SWITCH);
    auto receiver1 = sim.add_device("receiver1", sim::DeviceType::RECEIVER);
    auto receiver2 = sim.add_device("receiver2", sim::DeviceType::RECEIVER);
    auto receiver3 = sim.add_device("receiver3", sim::DeviceType::RECEIVER);

    add_links(sim, {{sender1, swtch1},
                    {sender2, swtch1},
                    {sender3, swtch2},
                    {swtch1, receiver1},
                    {swtch1, receiver2},
                    {swtch1, swtch2},
                    {swtch2, receiver2},
                    {swtch2, receiver3}});

    sim.recalculate_paths();

    spdlog::info("Recalculated paths");

    EXPECT_EQ(sender1->get_link_to_destination(swtch1)->get_to(), swtch1);
    EXPECT_EQ(sender1->get_link_to_destination(swtch2)->get_to(), swtch1);
    EXPECT_EQ(sender1->get_link_to_destination(receiver1)->get_to(), swtch1);
    EXPECT_EQ(sender1->get_link_to_destination(receiver2)->get_to(), swtch1);
    EXPECT_EQ(sender1->get_link_to_destination(receiver3)->get_to(), swtch1);

    EXPECT_EQ(sender2->get_link_to_destination(swtch1)->get_to(), swtch1);
    EXPECT_EQ(sender2->get_link_to_destination(swtch2)->get_to(), swtch1);
    EXPECT_EQ(sender2->get_link_to_destination(receiver1)->get_to(), swtch1);
    EXPECT_EQ(sender2->get_link_to_destination(receiver2)->get_to(), swtch1);
    EXPECT_EQ(sender2->get_link_to_destination(receiver3)->get_to(), swtch1);

    EXPECT_EQ(sender3->get_link_to_destination(swtch1)->get_to(), swtch2);
    EXPECT_EQ(sender3->get_link_to_destination(swtch2)->get_to(), swtch2);
    EXPECT_EQ(sender3->get_link_to_destination(receiver1)->get_to(), swtch2);
    EXPECT_EQ(sender3->get_link_to_destination(receiver2)->get_to(), swtch2);
    EXPECT_EQ(sender3->get_link_to_destination(receiver3)->get_to(), swtch2);

    EXPECT_EQ(swtch1->get_link_to_destination(swtch2)->get_to(), swtch2);
    EXPECT_EQ(swtch1->get_link_to_destination(sender1)->get_to(), sender1);
    EXPECT_EQ(swtch1->get_link_to_destination(sender2)->get_to(), sender2);
    EXPECT_EQ(swtch1->get_link_to_destination(receiver1)->get_to(), receiver1);
    EXPECT_EQ(swtch1->get_link_to_destination(receiver2)->get_to(), receiver2);
    EXPECT_EQ(swtch1->get_link_to_destination(receiver3)->get_to(), swtch2);

    EXPECT_EQ(swtch2->get_link_to_destination(sender1)->get_to(), swtch1);
    EXPECT_EQ(swtch2->get_link_to_destination(sender2)->get_to(), swtch1);
    EXPECT_EQ(swtch2->get_link_to_destination(sender3)->get_to(), sender3);
    EXPECT_EQ(swtch2->get_link_to_destination(receiver1)->get_to(), swtch1);
    EXPECT_EQ(swtch2->get_link_to_destination(receiver2)->get_to(), receiver2);
    EXPECT_EQ(swtch2->get_link_to_destination(receiver3)->get_to(), receiver3);

    EXPECT_EQ(receiver1->get_link_to_destination(sender1)->get_to(), swtch1);
    EXPECT_EQ(receiver1->get_link_to_destination(sender2)->get_to(), swtch1);
    EXPECT_EQ(receiver1->get_link_to_destination(sender3)->get_to(), swtch1);
    EXPECT_EQ(receiver1->get_link_to_destination(swtch1)->get_to(), swtch1);
    EXPECT_EQ(receiver1->get_link_to_destination(swtch2)->get_to(), swtch1);
    EXPECT_EQ(receiver1->get_link_to_destination(receiver2)->get_to(), swtch1);
    EXPECT_EQ(receiver1->get_link_to_destination(receiver3)->get_to(), swtch1);

    EXPECT_EQ(receiver2->get_link_to_destination(sender1)->get_to(), swtch1);
    EXPECT_EQ(receiver2->get_link_to_destination(sender2)->get_to(), swtch1);
    EXPECT_EQ(receiver2->get_link_to_destination(sender3)->get_to(), swtch2);
    EXPECT_EQ(receiver2->get_link_to_destination(swtch1)->get_to(), swtch1);
    EXPECT_EQ(receiver2->get_link_to_destination(swtch2)->get_to(), swtch2);
    EXPECT_EQ(receiver2->get_link_to_destination(receiver1)->get_to(), swtch1);
    EXPECT_EQ(receiver2->get_link_to_destination(receiver3)->get_to(), swtch2);

    EXPECT_EQ(receiver3->get_link_to_destination(sender1)->get_to(), swtch2);
    EXPECT_EQ(receiver3->get_link_to_destination(sender2)->get_to(), swtch2);
    EXPECT_EQ(receiver3->get_link_to_destination(sender3)->get_to(), swtch2);
    EXPECT_EQ(receiver3->get_link_to_destination(swtch1)->get_to(), swtch2);
    EXPECT_EQ(receiver3->get_link_to_destination(swtch2)->get_to(), swtch2);
    EXPECT_EQ(receiver3->get_link_to_destination(receiver1)->get_to(), swtch2);
    EXPECT_EQ(receiver3->get_link_to_destination(receiver2)->get_to(), swtch2);
}

/*
    sender1   sender2   sender3
       |         |          |
       1         2          3
       |         |          |
    switch1---switch2---switch3
         \                /
         switch4---switch5
         /          |   \
        /           |    \
       /            |     \
  receiver1  receiver2  receiver3
*/

TEST_F(RecalculatePaths, LoopTopology) {
    sim::Simulator sim;

    auto sender1 = sim.add_device("sender1", sim::DeviceType::SENDER);
    auto sender2 = sim.add_device("sender2", sim::DeviceType::SENDER);
    auto sender3 = sim.add_device("sender3", sim::DeviceType::SENDER);
    auto swtch1 = sim.add_device("switch1", sim::DeviceType::SWITCH);
    auto swtch2 = sim.add_device("switch2", sim::DeviceType::SWITCH);
    auto swtch3 = sim.add_device("switch3", sim::DeviceType::SWITCH);
    auto swtch4 = sim.add_device("switch4", sim::DeviceType::SWITCH);
    auto swtch5 = sim.add_device("switch5", sim::DeviceType::SWITCH);
    auto receiver1 = sim.add_device("receiver1", sim::DeviceType::RECEIVER);
    auto receiver2 = sim.add_device("receiver2", sim::DeviceType::RECEIVER);
    auto receiver3 = sim.add_device("receiver3", sim::DeviceType::RECEIVER);

    add_links(sim, {
                       {sender1, swtch1},
                       {sender2, swtch2},
                       {sender3, swtch3},
                       {swtch1, swtch2},
                       {swtch2, swtch3},
                       {swtch3, swtch5},
                       {swtch4, swtch5},
                       {swtch1, swtch4},
                       {receiver1, swtch4},
                       {receiver2, swtch5},
                       {receiver3, swtch5},
                   });

    sim.recalculate_paths();

    EXPECT_EQ(sender1->get_link_to_destination(swtch1)->get_to(), swtch1);
    EXPECT_EQ(sender1->get_link_to_destination(swtch2)->get_to(), swtch1);
    EXPECT_EQ(sender1->get_link_to_destination(swtch3)->get_to(), swtch1);
    EXPECT_EQ(sender1->get_link_to_destination(swtch4)->get_to(), swtch1);
    EXPECT_EQ(sender1->get_link_to_destination(swtch5)->get_to(), swtch1);
    EXPECT_EQ(sender1->get_link_to_destination(receiver1)->get_to(), swtch1);
    EXPECT_EQ(sender1->get_link_to_destination(receiver2)->get_to(), swtch1);
    EXPECT_EQ(sender1->get_link_to_destination(receiver3)->get_to(), swtch1);

    EXPECT_EQ(sender2->get_link_to_destination(swtch1)->get_to(), swtch2);
    EXPECT_EQ(sender2->get_link_to_destination(swtch2)->get_to(), swtch2);
    EXPECT_EQ(sender2->get_link_to_destination(swtch3)->get_to(), swtch2);
    EXPECT_EQ(sender2->get_link_to_destination(swtch4)->get_to(), swtch2);
    EXPECT_EQ(sender2->get_link_to_destination(swtch5)->get_to(), swtch2);
    EXPECT_EQ(sender2->get_link_to_destination(receiver1)->get_to(), swtch2);
    EXPECT_EQ(sender2->get_link_to_destination(receiver2)->get_to(), swtch2);
    EXPECT_EQ(sender2->get_link_to_destination(receiver3)->get_to(), swtch2);

    EXPECT_EQ(sender3->get_link_to_destination(swtch1)->get_to(), swtch3);
    EXPECT_EQ(sender3->get_link_to_destination(swtch2)->get_to(), swtch3);
    EXPECT_EQ(sender3->get_link_to_destination(swtch3)->get_to(), swtch3);
    EXPECT_EQ(sender3->get_link_to_destination(swtch4)->get_to(), swtch3);
    EXPECT_EQ(sender3->get_link_to_destination(swtch5)->get_to(), swtch3);
    EXPECT_EQ(sender3->get_link_to_destination(receiver1)->get_to(), swtch3);
    EXPECT_EQ(sender3->get_link_to_destination(receiver2)->get_to(), swtch3);
    EXPECT_EQ(sender3->get_link_to_destination(receiver3)->get_to(), swtch3);

    EXPECT_EQ(swtch1->get_link_to_destination(sender1)->get_to(), sender1);
    EXPECT_EQ(swtch1->get_link_to_destination(sender2)->get_to(), swtch2);
    EXPECT_EQ(swtch1->get_link_to_destination(sender3)->get_to(), swtch2);
    EXPECT_EQ(swtch1->get_link_to_destination(swtch2)->get_to(), swtch2);
    EXPECT_EQ(swtch1->get_link_to_destination(swtch3)->get_to(), swtch2);
    EXPECT_EQ(swtch1->get_link_to_destination(swtch4)->get_to(), swtch4);
    EXPECT_EQ(swtch1->get_link_to_destination(swtch5)->get_to(), swtch4);
    EXPECT_EQ(swtch1->get_link_to_destination(receiver1)->get_to(), swtch4);
    EXPECT_EQ(swtch1->get_link_to_destination(receiver2)->get_to(), swtch4);
    EXPECT_EQ(swtch1->get_link_to_destination(receiver3)->get_to(), swtch4);

    EXPECT_EQ(swtch2->get_link_to_destination(sender1)->get_to(), swtch1);
    EXPECT_EQ(swtch2->get_link_to_destination(sender2)->get_to(), sender2);
    EXPECT_EQ(swtch2->get_link_to_destination(sender3)->get_to(), swtch3);
    EXPECT_EQ(swtch2->get_link_to_destination(swtch1)->get_to(), swtch1);
    EXPECT_EQ(swtch2->get_link_to_destination(swtch3)->get_to(), swtch3);
    EXPECT_EQ(swtch2->get_link_to_destination(swtch4)->get_to(), swtch1);
    EXPECT_EQ(swtch2->get_link_to_destination(swtch5)->get_to(), swtch3);
    EXPECT_EQ(swtch2->get_link_to_destination(receiver1)->get_to(), swtch1);
    EXPECT_EQ(swtch2->get_link_to_destination(receiver2)->get_to(), swtch3);
    EXPECT_EQ(swtch2->get_link_to_destination(receiver3)->get_to(), swtch3);

    EXPECT_EQ(swtch3->get_link_to_destination(sender1)->get_to(), swtch2);
    EXPECT_EQ(swtch3->get_link_to_destination(sender2)->get_to(), swtch2);
    EXPECT_EQ(swtch3->get_link_to_destination(sender3)->get_to(), sender3);
    EXPECT_EQ(swtch3->get_link_to_destination(swtch1)->get_to(), swtch2);
    EXPECT_EQ(swtch3->get_link_to_destination(swtch2)->get_to(), swtch2);
    EXPECT_EQ(swtch3->get_link_to_destination(swtch4)->get_to(), swtch5);
    EXPECT_EQ(swtch3->get_link_to_destination(swtch5)->get_to(), swtch5);
    EXPECT_EQ(swtch3->get_link_to_destination(receiver1)->get_to(), swtch5);
    EXPECT_EQ(swtch3->get_link_to_destination(receiver2)->get_to(), swtch5);
    EXPECT_EQ(swtch3->get_link_to_destination(receiver3)->get_to(), swtch5);

    EXPECT_EQ(swtch4->get_link_to_destination(sender1)->get_to(), swtch1);
    EXPECT_EQ(swtch4->get_link_to_destination(sender2)->get_to(), swtch1);
    EXPECT_EQ(swtch4->get_link_to_destination(sender3)->get_to(), swtch5);
    EXPECT_EQ(swtch4->get_link_to_destination(swtch1)->get_to(), swtch1);
    EXPECT_EQ(swtch4->get_link_to_destination(swtch2)->get_to(), swtch1);
    EXPECT_EQ(swtch4->get_link_to_destination(swtch3)->get_to(), swtch5);
    EXPECT_EQ(swtch4->get_link_to_destination(swtch5)->get_to(), swtch5);
    EXPECT_EQ(swtch4->get_link_to_destination(receiver1)->get_to(), receiver1);
    EXPECT_EQ(swtch4->get_link_to_destination(receiver2)->get_to(), swtch5);
    EXPECT_EQ(swtch4->get_link_to_destination(receiver3)->get_to(), swtch5);

    EXPECT_EQ(swtch5->get_link_to_destination(sender1)->get_to(), swtch4);
    EXPECT_EQ(swtch5->get_link_to_destination(sender2)->get_to(), swtch3);
    EXPECT_EQ(swtch5->get_link_to_destination(sender3)->get_to(), swtch3);
    EXPECT_EQ(swtch5->get_link_to_destination(swtch1)->get_to(), swtch4);
    EXPECT_EQ(swtch5->get_link_to_destination(swtch2)->get_to(), swtch3);
    EXPECT_EQ(swtch5->get_link_to_destination(swtch3)->get_to(), swtch3);
    EXPECT_EQ(swtch5->get_link_to_destination(swtch4)->get_to(), swtch4);
    EXPECT_EQ(swtch5->get_link_to_destination(receiver1)->get_to(), swtch4);
    EXPECT_EQ(swtch5->get_link_to_destination(receiver2)->get_to(), receiver2);
    EXPECT_EQ(swtch5->get_link_to_destination(receiver3)->get_to(), receiver3);

    EXPECT_EQ(receiver1->get_link_to_destination(sender1)->get_to(), swtch4);
    EXPECT_EQ(receiver1->get_link_to_destination(sender2)->get_to(), swtch4);
    EXPECT_EQ(receiver1->get_link_to_destination(sender3)->get_to(), swtch4);
    EXPECT_EQ(receiver1->get_link_to_destination(swtch1)->get_to(), swtch4);
    EXPECT_EQ(receiver1->get_link_to_destination(swtch2)->get_to(), swtch4);
    EXPECT_EQ(receiver1->get_link_to_destination(swtch3)->get_to(), swtch4);
    EXPECT_EQ(receiver1->get_link_to_destination(swtch4)->get_to(), swtch4);
    EXPECT_EQ(receiver1->get_link_to_destination(swtch5)->get_to(), swtch4);
    EXPECT_EQ(receiver1->get_link_to_destination(receiver2)->get_to(), swtch4);
    EXPECT_EQ(receiver1->get_link_to_destination(receiver3)->get_to(), swtch4);

    EXPECT_EQ(receiver2->get_link_to_destination(sender1)->get_to(), swtch5);
    EXPECT_EQ(receiver2->get_link_to_destination(sender2)->get_to(), swtch5);
    EXPECT_EQ(receiver2->get_link_to_destination(sender3)->get_to(), swtch5);
    EXPECT_EQ(receiver2->get_link_to_destination(swtch1)->get_to(), swtch5);
    EXPECT_EQ(receiver2->get_link_to_destination(swtch2)->get_to(), swtch5);
    EXPECT_EQ(receiver2->get_link_to_destination(swtch3)->get_to(), swtch5);
    EXPECT_EQ(receiver2->get_link_to_destination(swtch4)->get_to(), swtch5);
    EXPECT_EQ(receiver2->get_link_to_destination(swtch5)->get_to(), swtch5);
    EXPECT_EQ(receiver2->get_link_to_destination(receiver1)->get_to(), swtch5);
    EXPECT_EQ(receiver2->get_link_to_destination(receiver3)->get_to(), swtch5);

    EXPECT_EQ(receiver3->get_link_to_destination(sender1)->get_to(), swtch5);
    EXPECT_EQ(receiver3->get_link_to_destination(sender2)->get_to(), swtch5);
    EXPECT_EQ(receiver3->get_link_to_destination(sender3)->get_to(), swtch5);
    EXPECT_EQ(receiver3->get_link_to_destination(swtch1)->get_to(), swtch5);
    EXPECT_EQ(receiver3->get_link_to_destination(swtch2)->get_to(), swtch5);
    EXPECT_EQ(receiver3->get_link_to_destination(swtch3)->get_to(), swtch5);
    EXPECT_EQ(receiver3->get_link_to_destination(swtch4)->get_to(), swtch5);
    EXPECT_EQ(receiver3->get_link_to_destination(swtch5)->get_to(), swtch5);
    EXPECT_EQ(receiver3->get_link_to_destination(receiver1)->get_to(), swtch5);
    EXPECT_EQ(receiver3->get_link_to_destination(receiver2)->get_to(), swtch5);
}

}  // namespace test
