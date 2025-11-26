#include "connection/mplb/round_robin_mplb.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

#include "../_mocks/flow_mock.hpp"

namespace test {

class RoundRobinMPLBTest : public ::testing::Test {
protected:
    using FlowPtr = std::shared_ptr<sim::IFlow>;
    using FlowList = std::vector<FlowPtr>;
    using FlowCount = std::map<FlowPtr, std::size_t, std::owner_less<FlowPtr>>;

    sim::RoundRobinMPLB mplb;

    static FlowPtr makeFlow(uint32_t pckts_count = 1,
                            SizeByte packet_size = SizeByte(64)) {
        auto flow = std::make_shared<test::FlowMock>(
            std::shared_ptr<sim::IHost>{}, packet_size,
            packet_size * pckts_count, TimeNs(0));
        return flow;
    }

    static FlowList makeFlows(std::size_t flow_count, uint32_t pckts_count = 1,
                              SizeByte packet_size = SizeByte(64)) {
        FlowList flows;
        flows.reserve(flow_count);
        for (std::size_t i = 0; i < flow_count; ++i)
            flows.emplace_back(makeFlow(pckts_count, packet_size));
        return flows;
    }

    FlowList pick(std::size_t k) {
        FlowList flows;
        flows.reserve(k);
        for (std::size_t i = 0; i < k; ++i) {
            auto flow =
                std::dynamic_pointer_cast<test::FlowMock>(mplb.select_flow());
            if (!flow) break;
            flow.get()->send_data(flow.get()->get_packet_size(), "");
            flows.emplace_back(flow);
        }
        return flows;
    }

    static FlowCount count(const FlowList& flows) {
        FlowCount flow_count;
        for (auto& flow : flows) ++flow_count[flow];
        return flow_count;
    }

    static void expectFair(const FlowCount& flow_count, const FlowPtr& flow_1,
                           const FlowPtr& flow_2) {
        int diff = static_cast<int>(flow_count.at(flow_1)) -
                   static_cast<int>(flow_count.at(flow_2));
        EXPECT_LE(std::abs(diff), 1);
    }
};

// Add two flows with the same quota and make sure that they are selected
// equally
TEST_F(RoundRobinMPLBTest, AddAndRotateFair) {
    // First flow can send 500 packets, second - also 500 packets
    auto flow_1 = makeFlow(500);
    auto flow_2 = makeFlow(500);
    mplb.add_flow(flow_1);
    mplb.add_flow(flow_2);
    auto flow_count = count(pick(1005));
    ASSERT_EQ(flow_count.size(), 2u);
    expectFair(flow_count, flow_1, flow_2);
}

// Add three flows and check that they are selected in turn in multiple loops
TEST_F(RoundRobinMPLBTest, RotateMultiCycle) {
    auto flows = makeFlows(3, 100);
    for (auto& flow : flows) mplb.add_flow(flow);
    auto flow_count = count(pick(305));
    ASSERT_EQ(flow_count.size(), 3u);
    for (auto& flow : flows) EXPECT_EQ(flow_count[flow], 100u);
}

// Re-adding the same flow must be ignored
TEST_F(RoundRobinMPLBTest, DuplicateAddIgnored) {
    auto flow = makeFlow(100);
    mplb.add_flow(flow);
    mplb.add_flow(flow);
    auto cnt = count(pick(200));
    ASSERT_EQ(cnt.size(), 1u);
    EXPECT_EQ(cnt[flow], 100u);
}

// Deleting the currently selected flow should correctly shift the pointer
TEST_F(RoundRobinMPLBTest, RemoveCurrentAdvances) {
    auto flows = makeFlows(3, 100);
    for (auto& flow : flows) mplb.add_flow(flow);
    auto first = mplb.select_flow();
    ASSERT_NE(first, nullptr);
    auto next = mplb.select_flow();
    ASSERT_NE(next, nullptr);
    mplb.remove_flow(next);
    auto after = mplb.select_flow();
    ASSERT_NE(after, nullptr);
    EXPECT_NE(after, next);
}

// If the last flow is deleted, the selection should return nullptr
TEST_F(RoundRobinMPLBTest, RemoveLast) {
    auto flow = makeFlow();
    mplb.add_flow(flow);
    ASSERT_EQ(mplb.select_flow(), flow);
    mplb.remove_flow(flow);
    EXPECT_EQ(mplb.select_flow(), nullptr);
}

// Remove two of the four flows and check that the remaining ones continue to be
// selected equally
TEST_F(RoundRobinMPLBTest, RemoveSomeKeepsFairness) {
    auto flows = makeFlows(4, 200);
    for (auto& flow : flows) mplb.add_flow(flow);
    mplb.remove_flow(flows[1]);
    mplb.remove_flow(flows[3]);
    auto flow_count = count(pick(200));
    ASSERT_EQ(flow_count.size(), 2u);
    expectFair(flow_count, flows[0], flows[2]);
}

// Clear all flows, then add them again and make sure everything works
TEST_F(RoundRobinMPLBTest, ClearResets) {
    auto flows = makeFlows(3, 10);
    for (auto& flow : flows) mplb.add_flow(flow);
    ASSERT_NE(mplb.select_flow(), nullptr);
    mplb.clear_flows();
    EXPECT_EQ(mplb.select_flow(), nullptr);
    for (auto& flow : flows) mplb.add_flow(flow);
    auto flow_count = count(pick(30));
    ASSERT_EQ(flow_count.size(), 3u);
}

// Check that selecting a flow with zero quota returns nullptr
TEST_F(RoundRobinMPLBTest, QuotaExhausted) {
    auto flow_1 = makeFlow(1), flow_2 = makeFlow(0);
    mplb.add_flow(flow_1);
    mplb.add_flow(flow_2);
    auto flow_count = count(pick(2));
    ASSERT_EQ(flow_count.size(), 1u);
    ASSERT_TRUE(flow_count.contains(flow_1));
    ASSERT_FALSE(flow_count.contains(flow_2));
}

// Check that selecting a flow with zero quota does not affect the round-robin
// cycle
TEST_F(RoundRobinMPLBTest, SingleFlowCycle) {
    auto flow = makeFlow(3);
    mplb.add_flow(flow);
    auto flow_count = count(pick(4));
    ASSERT_EQ(flow_count.size(), 1u);
    ASSERT_TRUE(flow_count.contains(flow));
    ASSERT_EQ(flow_count.at(flow), 3u);
}

}  // namespace test
