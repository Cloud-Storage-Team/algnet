#include <gtest/gtest.h>
#include <map>
#include <memory>
#include <vector>
#include <algorithm>
#include "connection/round_robin_mplb.hpp"
#include "../switch/flow_mock.hpp"

namespace test {

class RoundRobinMPLBTest : public ::testing::Test {
protected:
    using FlowPtr   = std::shared_ptr<sim::IFlow>;
    using FlowList  = std::vector<FlowPtr>;
    using FlowCount = std::map<FlowPtr, std::size_t, std::owner_less<FlowPtr>>;

    sim::RoundRobinMPLB mplb;

    static FlowPtr makeFlow() {
        return std::make_shared<test::FlowMock>(std::shared_ptr<sim::IHost>{});
    }

    static sim::FlowSample makeSample(uint32_t quota) {
        sim::FlowSample sample;
        sample.send_quota = quota;
        return sample;
    }

    static FlowList makeFlows(std::size_t flow_count) {
        FlowList flows;
        flows.reserve(flow_count);
        for (std::size_t i = 0; i < flow_count; ++i) flows.emplace_back(makeFlow());
        return flows;
    }

    FlowList pick(std::size_t k) {
        FlowList flows;
        flows.reserve(k);
        for (std::size_t i = 0; i < k; ++i) {
            auto flow = mplb.select_flow();
            if (!flow) break;
            flows.emplace_back(flow);
        }
        return flows;
    }

    static FlowCount count(const FlowList& flows) {
        FlowCount flow_count;
        for (auto& flow : flows) ++flow_count[flow];
        return flow_count;
    }

    static void expectFair(const FlowCount& flow_count, const FlowPtr& flow_1, const FlowPtr& flow_2) {
        int diff = static_cast<int>(flow_count.at(flow_1)) - static_cast<int>(flow_count.at(flow_2));
        EXPECT_LE(std::abs(diff), 1);
    }
};

// Add two flows with the same quota and make sure that they are selected equally
TEST_F(RoundRobinMPLBTest, AddAndRotateFair) {
    auto flow_1 = makeFlow(), flow_2 = makeFlow();
    mplb.add_flow(flow_1, makeSample(1000));
    mplb.add_flow(flow_2, makeSample(1000));
    auto flow_count = count(pick(1000));
    ASSERT_EQ(flow_count.size(), 2u);
    expectFair(flow_count, flow_1, flow_2);
}

// Add three flows and check that they are selected in turn in multiple loops
TEST_F(RoundRobinMPLBTest, RotateMultiCycle) {
    auto flows = makeFlows(3);
    for (auto& flow : flows) mplb.add_flow(flow, makeSample(100));
    auto flow_count = count(pick(300));
    ASSERT_EQ(flow_count.size(), 3u);
    for (auto& flow : flows) EXPECT_EQ(flow_count[flow], 100u);
}

// Re-adding the same flow must be ignored
TEST_F(RoundRobinMPLBTest, DuplicateAddIgnored) {
    auto flow = makeFlow();
    mplb.add_flow(flow, makeSample(1));
    mplb.add_flow(flow, makeSample(100));
    auto cnt = count(pick(50));
    ASSERT_EQ(cnt.size(), 1u);
    EXPECT_EQ(cnt[flow], 1u);
}

// Deleting the currently selected flow should correctly shift the pointer
TEST_F(RoundRobinMPLBTest, RemoveCurrentAdvances) {
    auto flow_1 = makeFlow(), flow_2 = makeFlow(), flow_3 = makeFlow();
    mplb.add_flow(flow_1, makeSample(100));
    mplb.add_flow(flow_2, makeSample(100));
    mplb.add_flow(flow_3, makeSample(100));
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
    mplb.add_flow(flow, makeSample(1));
    ASSERT_EQ(mplb.select_flow(), flow);
    mplb.remove_flow(flow);
    EXPECT_EQ(mplb.select_flow(), nullptr);
}

// Remove two of the four flows and check that the remaining ones continue to be selected equally
TEST_F(RoundRobinMPLBTest, RemoveSomeKeepsFairness) {
    auto flow_1 = makeFlow(), flow_2 = makeFlow(), flow_3 = makeFlow(), flow_4 = makeFlow();
    mplb.add_flow(flow_1, makeSample(200));
    mplb.add_flow(flow_2, makeSample(200));
    mplb.add_flow(flow_3, makeSample(200));
    mplb.add_flow(flow_4, makeSample(200));
    mplb.remove_flow(flow_2);
    mplb.remove_flow(flow_4);
    auto flow_count = count(pick(200));
    ASSERT_EQ(flow_count.size(), 2u);
    expectFair(flow_count, flow_1, flow_3);
}

// Clear all flows, then add them again and make sure everything works
TEST_F(RoundRobinMPLBTest, ClearResets) {
    auto flows = makeFlows(3);
    for (auto& flow : flows) mplb.add_flow(flow, makeSample(10));
    ASSERT_NE(mplb.select_flow(), nullptr);
    mplb.clear_flows();
    EXPECT_EQ(mplb.select_flow(), nullptr);
    for (auto& flow : flows) mplb.add_flow(flow, makeSample(100));
    auto flow_count = count(pick(300));
    ASSERT_EQ(flow_count.size(), 3u);
}

// Check that selecting a flow with zero quota returns nullptr
TEST_F(RoundRobinMPLBTest, QuotaExhausted) {
    auto flow_1 = makeFlow(), flow_2 = makeFlow();
    mplb.add_flow(flow_1, makeSample(1));
    mplb.add_flow(flow_2, makeSample(0));
    EXPECT_EQ(mplb.select_flow(), flow_1);
    EXPECT_EQ(mplb.select_flow(), nullptr);
}

// Check that selecting a flow with zero quota does not affect the round-robin cycle
TEST_F(RoundRobinMPLBTest, SingleFlowCycle) {
    auto flow = makeFlow();
    mplb.add_flow(flow, makeSample(3));
    EXPECT_EQ(mplb.select_flow(), flow);
    EXPECT_EQ(mplb.select_flow(), flow);
    EXPECT_EQ(mplb.select_flow(), flow);
    EXPECT_EQ(mplb.select_flow(), nullptr);
}

} // namespace test
