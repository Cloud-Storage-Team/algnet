#include <gtest/gtest.h>

#include "device/device.hpp"
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

static void check_pairwise_reachability(
    std::initializer_list<std::shared_ptr<sim::IRoutingDevice>> devices) {
    for (auto src : devices) {
        for (auto dest : devices) {
            if (src != dest) {
                EXPECT_TRUE(check_reachability(src, dest));
            }
        }
    }
}

TEST_F(RecalculatePaths, TrivialTopology) {
    TestSimulator sim;

    Id sender_id = sim.add_sender("sender");
    Id switch_id = sim.add_switch("switch");
    Id receiver_id = sim.add_receiver("receiver");

    auto sender = sim.get_sender(sender_id);
    auto swtch = sim.get_switch(switch_id);
    auto receiver = sim.get_receiver(receiver_id);

    add_two_way_links(sim, {{"sender", "switch"}, {"switch", "receiver"}});
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
    TestSimulator sim;

    Id sender1_id = sim.add_sender("sender1");
    Id sender2_id = sim.add_sender("sender2");
    Id switch_id = sim.add_switch("switch");
    Id receiver_id = sim.add_receiver("receiver");
    
    auto sender1 = sim.get_sender(sender1_id);
    auto sender2 = sim.get_sender(sender2_id);
    auto swtch = sim.get_switch(switch_id);
    auto receiver = sim.get_receiver(receiver_id);

    add_two_way_links(sim,
                      {{"sender1", "switch"}, {"sender2", "switch"}, {"switch", "receiver"}});

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
    TestSimulator sim;

    Id sender1_id = sim.add_sender("sender1");
    Id sender2_id = sim.add_sender("sender2");
    Id sender3_id = sim.add_sender("sender3");
    Id switch1_id = sim.add_switch("switch1");
    Id switch2_id = sim.add_switch("switch2");
    Id receiver1_id = sim.add_receiver("receiver1");
    Id receiver2_id = sim.add_receiver("receiver2");
    Id receiver3_id = sim.add_receiver("receiver3");

    auto sender1 = sim.get_sender(sender1_id);
    auto sender2 = sim.get_sender(sender2_id);
    auto sender3 = sim.get_sender(sender3_id);
    auto swtch1 = sim.get_switch(switch1_id);
    auto swtch2 = sim.get_switch(switch2_id);
    auto receiver1 = sim.get_receiver(receiver1_id);
    auto receiver2 = sim.get_receiver(receiver2_id);
    auto receiver3 = sim.get_receiver(receiver3_id);


    add_two_way_links(sim, {{"sender1", "switch1"},
                            {"sender2", "switch1"},
                            {"sender3", "switch2"},
                            {"switch1", "receiver1"},
                            {"switch1", "receiver2"},
                            {"switch1", "switch2"},
                            {"switch2", "receiver2"},
                            {"switch2", "receiver3"}});

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
    TestSimulator sim;

    Id sender1_id = sim.add_sender("sender1");
    Id sender2_id = sim.add_sender("sender2");
    Id sender3_id = sim.add_sender("sender3");
    Id switch1_id = sim.add_switch("switch1");
    Id switch2_id = sim.add_switch("switch2");
    Id switch3_id = sim.add_switch("switch3");
    Id switch4_id = sim.add_switch("switch4");
    Id switch5_id = sim.add_switch("switch5");
    Id receiver1_id = sim.add_receiver("receiver1");
    Id receiver2_id = sim.add_receiver("receiver2");
    Id receiver3_id = sim.add_receiver("receiver3");

    auto sender1 = sim.get_sender(sender1_id);
    auto sender2 = sim.get_sender(sender2_id);
    auto sender3 = sim.get_sender(sender3_id);
    auto swtch1 = sim.get_switch(switch1_id);
    auto swtch2 = sim.get_switch(switch2_id);
    auto swtch3 = sim.get_switch(switch3_id);
    auto swtch4 = sim.get_switch(switch4_id);
    auto swtch5 = sim.get_switch(switch5_id);
    auto receiver1 = sim.get_receiver(receiver1_id);
    auto receiver2 = sim.get_receiver(receiver2_id);
    auto receiver3 = sim.get_receiver(receiver3_id);

    add_two_way_links(sim, {
                               {"sender1", "switch1"},
                               {"sender2", "switch2"},
                               {"sender3", "switch3"},
                               {"switch1", "switch2"},
                               {"switch2", "switch3"},
                               {"switch3", "switch5"},
                               {"switch4", "switch5"},
                               {"switch1", "switch4"},
                               {"receiver1", "switch4"},
                               {"receiver2", "switch5"},
                               {"receiver3", "switch5"},
                           });

    sim.recalculate_paths();

    check_pairwise_reachability({sender1, sender2, sender3, swtch1, swtch2,
                                 swtch4, swtch5, receiver1, receiver2,
                                 receiver3});
}

}  // namespace test
