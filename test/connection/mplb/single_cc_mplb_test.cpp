#include "connection/mplb/single_cc/single_cc_mplb.hpp"

#include <gtest/gtest.h>

#include "../../_mocks/cc_mock.hpp"
#include "../../_mocks/new_flow_mock.hpp"
#include "../../_mocks/path_chooser_mock.hpp"
#include "scheduler.hpp"

namespace test {
class SingleCCMplbTest : public ::testing::Test {};

TEST_F(SingleCCMplbTest, SimpleSend) {
    std::set<std::shared_ptr<sim::INewFlow> > pathes = {
        std::make_shared<NewFlowMock>()};

    std::unique_ptr<sim::IPathChooser> path_chooser =
        std::make_unique<PathChooserMock>(pathes);

    std::unique_ptr<sim::ITcpCC> cc = std::make_unique<CCMock>(1, TimeNs(0));

    SizeByte packet_size(1500);

    auto mplb = sim::SingleCCMplb::create_shared(
        std::move(cc), std::move(path_chooser), packet_size);
    bool delivered = false;
    OnDeliveryCallback callback = [&delivered]() { delivered = true; };
    auto res = mplb->send_data(
        sim::Data(sim::DataId{"", std::nullopt}, packet_size), callback);
    EXPECT_TRUE(res.has_value()) << res.error();

    {
        sim::MPLBContext ctx = mplb->get_context();
        // check just quota because currently MPLB may not trigger data
        // delivery, just schedule it
        EXPECT_EQ(ctx.sending_quota, SizeByte(0));
    }

    while (sim::Scheduler::get_instance().tick()) {
    }

    EXPECT_TRUE(delivered)
        << "Callback on data delivery should be triggered but in was not";
    {
        sim::MPLBContext ctx = mplb->get_context();
        EXPECT_EQ(ctx.sent_data_size, packet_size);
        EXPECT_EQ(ctx.delivered_data_size, packet_size);
        EXPECT_EQ(ctx.sending_quota, SizeByte(0));
    }
}

TEST_F(SingleCCMplbTest, IncorrectSend) {
    std::set<std::shared_ptr<sim::INewFlow> > pathes = {
        std::make_shared<NewFlowMock>()};

    std::unique_ptr<sim::IPathChooser> path_chooser =
        std::make_unique<PathChooserMock>(pathes);

    std::unique_ptr<sim::ITcpCC> cc = std::make_unique<CCMock>(1, TimeNs(0));

    SizeByte quota(1500);
    SizeByte big_packet_size(3000);

    auto mplb = sim::SingleCCMplb::create_shared(
        std::move(cc), std::move(path_chooser), quota);
    bool delivered = false;
    OnDeliveryCallback callback = [&delivered]() { delivered = true; };
    auto res = mplb->send_data(
        sim::Data(sim::DataId{"", std::nullopt}, big_packet_size), callback);
    EXPECT_FALSE(res.has_value()) << "Data should not be delivered, but it was";

    while (sim::Scheduler::get_instance().tick()) {
    }

    EXPECT_FALSE(delivered)
        << "On delivery callback should not be triggered, but it was";
    sim::MPLBContext ctx = mplb->get_context();
    EXPECT_EQ(ctx.sent_data_size, SizeByte(0));
    EXPECT_EQ(ctx.delivered_data_size, SizeByte(0));
    EXPECT_EQ(ctx.sending_quota, quota)
        << "Quota should not be changed on incorrect data delivery";
}

}  // namespace test
