#include <gtest/gtest.h>

#include "connection/connection_impl.hpp"
#include "connection/flow/tcp/basic/basic_cc.hpp"
#include "connection/mplb/round_robin_mplb.hpp"
#include "scenario/action/send_data_action.hpp"
#include "scenario/scenario.hpp"
#include "simulator.hpp"
#include "utils.hpp"

namespace test {

namespace {

std::shared_ptr<sim::TcpFlow> add_connection_with_single_flow(
    sim::Simulator& sim, const Id& conn_id,
    const std::shared_ptr<sim::Host>& sender,
    const std::shared_ptr<sim::Host>& receiver, SizeByte packet_size) {
    auto connection = std::make_shared<sim::ConnectionImpl>(
        conn_id, sender, receiver, std::make_shared<sim::RoundRobinMPLB>());

    auto flow =
        sim::TcpFlow::create(conn_id + "_flow1", connection,
                             std::make_unique<sim::BasicCC>(), packet_size);

    connection->add_flow(flow);

    sim.add_connection(connection);
    return flow;
}
}  // namespace

class Start : public testing::Test {
public:
    void TearDown() override { sim::IdentifierFactory::get_instance().clear(); }
    void SetUp() override {};
};

TEST_F(Start, TrivialTopology) {
    sim::Simulator sim;
    auto sender = std::make_shared<sim::Host>("sender");
    auto swtch = std::make_shared<sim::Switch>("switch");
    auto receiver = std::make_shared<sim::Host>("receiver");

    sim.add_host(sender);
    sim.add_switch(swtch);
    sim.add_host(receiver);

    add_two_way_links(sim, {{sender, swtch}, {swtch, receiver}});

    constexpr SizeByte packet_size(1024);
    constexpr SizeByte data_to_send = SizeByte(1024);

    auto flow = add_connection_with_single_flow(sim, "conn1", sender, receiver,
                                                packet_size);

    auto scenario = sim::Scenario();
    std::vector<std::weak_ptr<sim::IConnection>> conns;

    for (const auto& connection : sim.get_connections()) {
        conns.push_back(connection);
    }

    scenario.add_action(std::make_unique<sim::SendDataAction>(
        TimeNs(0), data_to_send, conns, 1, TimeNs(0)));

    sim.set_scenario(std::move(scenario));

    sim.start();

    ASSERT_EQ(flow->get_delivered_bytes(), data_to_send);
}

TEST_F(Start, ThreeToOneTopology) {
    sim::Simulator sim;

    auto swtch = std::make_shared<sim::Switch>("switch");
    auto receiver = std::make_shared<sim::Host>("receiver");
    sim.add_switch(swtch);
    sim.add_host(receiver);

    std::vector<std::shared_ptr<sim::Host>> senders;
    for (int i = 1; i <= 3; ++i) {
        auto s = std::make_shared<sim::Host>("sender" + std::to_string(i));
        sim.add_host(s);
        senders.push_back(s);
        add_two_way_links(sim, {{s, swtch}});
    }
    add_two_way_links(sim, {{swtch, receiver}});

    const SizeByte packet_size{10};

    const std::uint32_t pkts[] = {10, 50, 100};

    std::vector<std::shared_ptr<sim::TcpFlow>> flows;
    std::unordered_map<std::string, SizeByte> data_to_send_map;

    for (int i = 0; i < 3; ++i) {
        auto flow =
            add_connection_with_single_flow(sim, "conn" + std::to_string(i + 1),
                                            senders[i], receiver, packet_size);
        flows.push_back(flow);
        data_to_send_map.emplace("conn" + std::to_string(i + 1),
                                 SizeByte(pkts[i] * packet_size));
    }

    auto scenario = sim::Scenario();

    for (const auto& connection : sim.get_connections()) {
        const auto size = data_to_send_map.at(connection->get_id());

        std::vector<std::weak_ptr<sim::IConnection>> conns;
        conns.emplace_back(connection);

        scenario.add_action(std::make_unique<sim::SendDataAction>(
            TimeNs(0), size, conns, 1, TimeNs(0)));
    }
    sim.set_scenario(std::move(scenario));

    sim.start();

    for (int i = 0; i < 3; ++i) {
        const auto conn_id = "conn" + std::to_string(i + 1);
        const auto expected = data_to_send_map.at(conn_id);
        const auto actual = flows[i]->get_delivered_bytes();
        ASSERT_EQ(actual, expected) << conn_id << " mismatch";
    }
}

}  // namespace test