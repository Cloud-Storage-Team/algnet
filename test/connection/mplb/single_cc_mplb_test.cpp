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

    auto mplb = sim::SingleCCMplb::create_shared(std::move(cc),
                                                 std::move(path_chooser));
    bool delivered = false;
    OnDeliveryCallback callback = [&delivered]() { delivered = true; };
    auto res = mplb->send_data(
        sim::Data(sim::DataId{"", std::nullopt}, packet_size), callback);
    EXPECT_TRUE(res.has_value()) << res.error();

    while (sim::Scheduler::get_instance().tick()) {
    }

    EXPECT_TRUE(delivered);
}

}  // namespace test
