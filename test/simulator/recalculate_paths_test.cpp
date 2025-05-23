#include <gtest/gtest.h>

#include "device/device.hpp"
#include "simulator.hpp"
#include "flow_mock.hpp"
#include "utils.hpp"
#include "logger/logger.hpp"

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

static void check_pairwise_reachability(
    std::initializer_list<std::shared_ptr<sim::IRoutingDevice>> devices) {
    for (auto src : devices) {
        for (auto dest : devices) {
            if (src != dest) {
                // FlowMock fake_flow = FlowMock(dest);
                sim::Packet packet_to_dest = sim::Packet(sim::PacketType::DATA, 0, nullptr, src->get_id(), dest->get_id());
                EXPECT_TRUE(check_reachability(src, packet_to_dest));
            }
        }
    }
}

TEST_F(RecalculatePaths, TrivialTopology) {
    sim::BasicSimulator sim;

    auto sender = sim.add_sender("sender");
    auto swtch = sim.add_switch("switch");
    auto receiver = sim.add_receiver("receiver");

    add_two_way_links(sim, {{sender, swtch}, {swtch, receiver}});
    sim.recalculate_paths();

    check_pairwise_reachability({sender, swtch, receiver});
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
    sim::BasicSimulator sim;

    auto sender1 = sim.add_sender("sender1");
    auto sender2 = sim.add_sender("sender2");
    auto swtch = sim.add_switch("switch");
    auto receiver = sim.add_receiver("receiver");
    
    add_two_way_links(sim,
                      {{sender1, swtch}, {sender2, swtch}, {swtch, receiver}});

    sim.recalculate_paths();

    check_pairwise_reachability({sender1, sender2, swtch, receiver});
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
    sim::BasicSimulator sim;

    auto sender1 = sim.add_sender("sender1");
    auto sender2 = sim.add_sender("sender2");
    auto sender3 = sim.add_sender("sender3");
    auto swtch1 = sim.add_switch("switch1");
    auto swtch2 = sim.add_switch("switch2");
    auto receiver1 = sim.add_receiver("receiver1");
    auto receiver2 = sim.add_receiver("receiver2");
    auto receiver3 = sim.add_receiver("receiver3");

    add_two_way_links(sim, {{sender1, swtch1},
                            {sender2, swtch1},
                            {sender3, swtch2},
                            {swtch1, receiver1},
                            {swtch1, receiver2},
                            {swtch1, swtch2},
                            {swtch2, receiver2},
                            {swtch2, receiver3}});

    sim.recalculate_paths();

    check_pairwise_reachability({sender1, sender2, sender3, swtch1, swtch2,
                                 receiver1, receiver2, receiver3});
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
    sim::BasicSimulator sim;

    auto sender1 = sim.add_sender("sender1");
    auto sender2 = sim.add_sender("sender2");
    auto sender3 = sim.add_sender("sender3");
    auto swtch1 = sim.add_switch("switch1");
    auto swtch2 = sim.add_switch("switch2");
    auto swtch3 = sim.add_switch("switch3");
    auto swtch4 = sim.add_switch("switch4");
    auto swtch5 = sim.add_switch("switch5");
    auto receiver1 = sim.add_receiver("receiver1");
    auto receiver2 = sim.add_receiver("receiver2");
    auto receiver3 = sim.add_receiver("receiver3");

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

    check_pairwise_reachability({sender1, sender2, sender3, swtch1, swtch2,
                                 swtch4, swtch5, receiver1, receiver2,
                                 receiver3});
}

}  // namespace test
