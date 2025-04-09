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

    add_two_way_links(sim, {{sender, swtch}, {swtch, receiver}});

    sim.recalculate_paths();

    auto devices = std::vector<std::shared_ptr<sim::IRoutingDevice>>{sender, swtch, receiver};

    for (auto src : devices) {
        for (auto dest : devices) {
            if (src != dest) {
                EXPECT_TRUE(check_reachability(src, dest));
            }
        }
    }
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

    add_two_way_links(sim, {{sender1, swtch}, {sender2, swtch}, {swtch, receiver}});

    sim.recalculate_paths();

    auto devices = std::vector<std::shared_ptr<sim::IRoutingDevice>>{sender1, sender2, swtch, receiver};

    for (auto src : devices) {
        for (auto dest : devices) {
            if (src != dest) {
                EXPECT_TRUE(check_reachability(src, dest));
            }
        }
    }
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

    add_two_way_links(sim, {{sender1, swtch1},
                    {sender2, swtch1},
                    {sender3, swtch2},
                    {swtch1, receiver1},
                    {swtch1, receiver2},
                    {swtch1, swtch2},
                    {swtch2, receiver2},
                    {swtch2, receiver3}});

    sim.recalculate_paths();

    auto devices = std::vector<std::shared_ptr<sim::IRoutingDevice>>{sender1, sender2,   sender3,   swtch1,
                    swtch2,  receiver1, receiver2, receiver3};

    for (auto src : devices) {
        for (auto dest : devices) {
            if (src != dest) {
                EXPECT_TRUE(check_reachability(src, dest));
            }
        }
    }
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

    add_two_way_links(sim, {
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

    auto devices = std::vector<std::shared_ptr<sim::IRoutingDevice>>{sender1, sender2, sender3,   swtch1,    swtch2,
                    swtch4,  swtch5,  receiver1, receiver2, receiver3};

    for (auto src : devices) {
        for (auto dest : devices) {
            if (src != dest) {
                EXPECT_TRUE(check_reachability(src, dest));
            }
        }
    }
}

}  // namespace test
