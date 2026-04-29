#include "network/connection/mplb/rdma/dcqcn.hpp"

#include <gtest/gtest.h>

#include "scheduler/scheduler.hpp"

namespace sim {
namespace test2 {

struct Fixture : public ::testing::Test {
    Fixture() : params() {
        // задать удобные параметры для детерминированного тестирования
        params.rpg_time_reset = TimeUs(100);
        params.rpg_byte_reset = SizeByte(1000);
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

// 1) start() ставит таймеры (проверим, что события запланированы и первый
// alpha обновится)
TEST_F(Fixture, RateIncrease) {
    Scheduler& sched = Scheduler::get_instance();
    DCQCN cc(params);
    cc.start();

    SpeedMbps initial_rate = cc.get_rate();

    // one additive increase should be triggered
    sched.tick_to(
        params.rpg_time_reset);  // dce_tcp_rtt = 10us => первый alpha через
                                 // 10us; убедимся, что до 5us не выполнено

    SpeedMbps new_rate = cc.get_rate();
    EXPECT_GT(new_rate, initial_rate);
}

// // 2) on_data_delivery: после накопления rpg_byte_reset должно вызвать
// // on_rate_increase_event
// TEST_F(Fixture, RateIncreaseByBytes) {
//     DCQCN cc(params);
//     cc.start();

//     // начальная скорость — rpg_min_rate
//     auto before = cc.get_rate();

//     // доставляем ровно rpg_byte_reset -> должно инкрементнуть целевую и
//     // обновить текущую
//     cc.on_data_delivery(params.rpg_byte_reset);
//     auto after = cc.get_rate();
//     EXPECT_GT(after, before);
// }

// // 3) on_rate_increase_event: проверка фаз (AI vs HAI)
// // Нужна имитация нескольких итераций: увеличиваем time_counter и
// byte_counter
// // вручную через deliveries/time advance
// TEST_F(Fixture, AdditiveAndHyperIncreasePhases) {
//     DCQCN cc(params);
//     cc.start();

//     // Сценарий: сначала одно событие -> additive increase
//     cc.on_data_delivery(params.rpg_byte_reset);
//     auto rate1 = cc.get_rate();

//     // имитируем много событий, чтобы перейти в HAI
//     for (int i = 0; i < 5; ++i)
//     cc.on_data_delivery(params.rpg_byte_reset); auto rate2 =
//     cc.get_rate();

//     EXPECT_GT(rate2, rate1);
// }

// // 4) on_cnp + rate reduction: при получении CNP и срабатывании monitor
// period
// // текущая скорость должна уменьшиться
// TEST_F(Fixture, RateDecreaseOnCnp) {
//     DCQCN cc(params);
//     cc.start();

//     // предварительно поднимем скорость
//     cc.on_data_delivery(params.rpg_byte_reset);
//     cc.on_data_delivery(params.rpg_byte_reset);
//     auto up = cc.get_rate();

//     // получаем CNP
//     cc.on_cnp();

//     // продвигаем время вперёд чтобы сработал rate_reduce_monitor_period
//     sched.tick_to(now_ns(25));  // period = 20us
//     sched.run_pending_tasks();  // выполнит on_rate_reduce_monitor_period
//     auto down = cc.get_rate();

//     EXPECT_LT(down, up);
// }

// // 5) stop() — дальнейшие события игнорируются
// TEST_F(Fixture, StopPreventsFurtherChanges) {
//     DCQCN cc(params);
//     cc.start();

//     cc.stop();
//     cc.on_data_delivery(params.rpg_byte_reset * 10);
//     auto rate_after = cc.get_rate();

//     // скорость не должна измениться после stop (равна минимальной
//     установленной
//     // при ctor)
//     EXPECT_EQ(rate_after, params.rpg_min_rate);
// }

}  // namespace test2
}  // namespace sim