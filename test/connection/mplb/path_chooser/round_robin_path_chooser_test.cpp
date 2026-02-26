#include "connection/mplb/path_chooser/round_robin/round_robin_path_chooser.hpp"

#include <gtest/gtest.h>

#include "../../../_mocks/flow_gmock.hpp"

namespace test {

struct RRParams {
    size_t flows_count;
    size_t repeat_count;
};

class RoundRobinPathChooserParamTest
    : public ::testing::TestWithParam<RRParams> {};

using ::testing::NiceMock;

TEST_P(RoundRobinPathChooserParamTest, ChoosesFlowsAlmostUniformly) {
    const RRParams p = GetParam();
    ASSERT_GT(p.flows_count, 0u);

    sim::IPathChooser::FlowsSet flows;
    std::vector<std::shared_ptr<NiceMock<NewFlowGMock>>> concrete_flows;

    for (size_t i = 0; i < p.flows_count; ++i) {
        auto flow =
            std::make_shared<NiceMock<NewFlowGMock>>(fmt::format("Flow_{}", i));
        std::shared_ptr<sim::INewFlow> as_base = flow;
        ASSERT_TRUE(flows.emplace(flow->name, as_base).second);
        concrete_flows.emplace_back(flow);
    }

    sim::RoundRobinPathChooser chooser(flows);

    std::unordered_map<std::shared_ptr<sim::INewFlow>, size_t> counts;
    counts.reserve(p.flows_count);

    for (size_t i = 0; i < p.repeat_count; ++i) {
        auto chosen = chooser.choose_flow();
        ASSERT_TRUE(chosen) << "choose_flow() returned nulltr on iter " << i;
        counts[chosen]++;
    }

    const size_t q = p.repeat_count / p.flows_count;
    const size_t r = p.repeat_count % p.flows_count;

    // It is expected that exactly r flow met q+1 times, rest (flows_count - r)
    // flows should met q times

    size_t have_q = 0;
    size_t have_q_plus_1 = 0;
    size_t total = 0;

    for (const auto& flow : concrete_flows) {
        const size_t flow_count = counts[flow];
        total += flow_count;

        if (flow_count == q) {
            ++have_q;
        } else if (flow_count == q + 1) {
            ++have_q_plus_1;
        } else {
            ADD_FAILURE() << "Incorrect distribution: one one the flow"
                          << flow->name << " count=" << flow_count
                          << ", but expected " << q << " or " << (q + 1)
                          << " (flows=" << p.flows_count
                          << ", repeats=" << p.repeat_count << ")";
        }
    }

    EXPECT_EQ(total, p.repeat_count);

    EXPECT_EQ(have_q_plus_1, r) << "Expected that exactly r = repeat_count % "
                                   "flows_count flows are chooer (q+1) times";

    EXPECT_EQ(have_q, p.flows_count - r)
        << "Expected that exactly flows_count - r (wrere r = repeat_count % "
           "flows_count) flows are chooer q times";
}

static std::string ParamName(const ::testing::TestParamInfo<RRParams>& info) {
    return "flows" + std::to_string(info.param.flows_count) + "_repeat" +
           std::to_string(info.param.repeat_count);
}

INSTANTIATE_TEST_SUITE_P(Fairness, RoundRobinPathChooserParamTest,
                         ::testing::Values(RRParams{1, 10}, RRParams{2, 1},
                                           RRParams{2, 5}, RRParams{3, 10},
                                           RRParams{4, 9}, RRParams{5, 100},
                                           RRParams{8, 1000}),
                         ParamName);

}  // namespace test
