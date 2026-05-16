#include "network/connection/rdma/dcqcn/dcqcn.hpp"

#include <gtest/gtest.h>

#include "scheduler/scheduler.hpp"
#include "utils/defer.hpp"

namespace sim {
namespace test2 {

struct Fixture : public ::testing::Test {
    Fixture() : params() {
        // задать удобные параметры для детерминированного тестирования
        params.rpg_time_reset = TimeUs(100);
        params.rpg_byte_reset = SizeByte(1000ul);
        params.rpg_threshold = 1;
        params.rpg_ai_rate = SpeedMbps(10);
        params.rpg_hai_rate = SpeedMbps(100);
        params.dce_tcp_rtt = TimeUs(10);
        params.rate_reduce_monitor_period = TimeUs(20);
        params.initial_alpha_value = 1023;
        params.dce_tcp_g = 1019;
        params.rpg_min_rate = SpeedMbps(100);
        params.rpg_min_dec_fac = 0.5;
    }

    void TearDown() override { Scheduler::get_instance().clear(); };
    void SetUp() override { Scheduler::get_instance().clear(); }

    ParamsDQCCN params;
};

// Helper: устанавливаем начальный момент времени
TimeNs now_ns(int us) { return TimeNs(us * 1000); }

TEST_F(Fixture, AdditiveIncreaseByTimer) {
    DCQCN cc(params);
    cc.start();
    utils::Defer defer([&cc]() { cc.stop(); });

    SpeedMbps before = cc.get_rate();

    // one additive increase should be triggered
    Scheduler::get_instance().tick_to(params.rpg_time_reset);

    SpeedMbps new_rate = cc.get_rate();
    EXPECT_GT(new_rate, before);
}

TEST_F(Fixture, AdditiveIncreaseByBytes) {
    DCQCN cc(params);
    cc.start();
    utils::Defer defer([&cc]() { cc.stop(); });

    SpeedMbps before = cc.get_rate();

    cc.on_data_delivery(params.rpg_byte_reset);
    SpeedMbps after = cc.get_rate();
    EXPECT_GT(after, before);
}

TEST_F(Fixture, AdditiveAndHyperIncrease) {
    DCQCN cc(params);
    cc.start();
    utils::Defer defer([&cc]() { cc.stop(); });

    SpeedMbps rate_start = cc.get_rate();

    // one additive increase
    cc.on_data_delivery(params.rpg_byte_reset);

    SpeedGbps rate_ai = cc.get_rate();
    EXPECT_GT(rate_ai, rate_start);

    // two more additive increase
    Scheduler::get_instance().tick_to(params.rpg_time_reset * 2);

    SpeedMbps rate_before_hai = cc.get_rate();

    // one hyper increase
    cc.on_data_delivery(params.rpg_byte_reset);

    SpeedMbps rate_hai = cc.get_rate();

    EXPECT_GT(rate_hai, rate_before_hai);

    static constexpr double thresh = 2.0;

    EXPECT_GT(rate_hai - rate_before_hai, thresh * (rate_ai - rate_start));
}

TEST_F(Fixture, RateDecreaseOnCnp) {
    DCQCN cc(params);
    cc.start();
    utils::Defer defer([&cc]() { cc.stop(); });

    // increase speed
    cc.on_data_delivery(params.rpg_byte_reset);
    cc.on_data_delivery(params.rpg_byte_reset);
    SpeedMbps up = cc.get_rate();

    cc.on_cnp();

    Scheduler::get_instance().tick_to(params.rate_reduce_monitor_period);
    SpeedMbps down = cc.get_rate();

    EXPECT_LT(down, up);
}

}  // namespace test2
}  // namespace sim