#include <gtest/gtest.h>

#include "connection/flow/tcp/new_tcp_flow.hpp"
#include "connection/flow/tcp/tahoe/tcp_tahoe_cc.hpp"
#include "connection/mplb/path_chooser/round_robin/round_robin_path_chooser.hpp"
#include "connection/mplb/single_cc/single_cc_mplb.hpp"
#include "connection/new_connection.hpp"
#include "device/host.hpp"
#include "link/link.hpp"
#include "packet.hpp"
#include "scheduler.hpp"

namespace test {
class ComplexConnectionTest : public testing::Test {
public:
    void TearDown() override { sim::Scheduler::get_instance().clear(); };
    void SetUp() override {
    sim::Scheduler::get_instance().clear();
        sender->add_outlink(fwd_link);
        sender->update_routing_table(receiver->get_id(), fwd_link, 1);
        receiver->add_inlink(fwd_link);

        receiver->add_outlink(back_link);
        receiver->update_routing_table(sender->get_id(), back_link, 1);
        sender->add_inlink(back_link);
    };

    // hosts
    std::shared_ptr<sim::Host> sender = std::make_shared<sim::Host>("sender");
    std::shared_ptr<sim::Host> receiver = std::make_shared<sim::Host>("receiver");

    // links
    std::shared_ptr<sim::Link> fwd_link =
        std::make_shared<sim::Link>("fwd_link", sender, receiver);
    std::shared_ptr<sim::Link> back_link =
        std::make_shared<sim::Link>("back_link", receiver, sender);

    std::shared_ptr<sim::NewTcpFlow> flow =
        sim::NewTcpFlow::create_shared("flow", sender, receiver);

    // mplb
    std::shared_ptr<sim::SingleCCMplb> mplb = sim::SingleCCMplb::create_shared(
        std::make_unique<sim::TcpTahoeCC>(),
        std::make_unique<sim::RoundRobinPathChooser>(
            sim::IPathChooser::FlowsSet({flow})));

    // connection
    std::shared_ptr<sim::NewConnection> connection =
        sim::NewConnection::create_shared("connection", mplb);
};

TEST_F(ComplexConnectionTest, SendOnePortion) {
    size_t count_callback_called = 0;

    SizeByte portion_size(10000);

    auto callback = [&]() { count_callback_called++; };

    auto res = connection->send_data(
        sim::Data(sim::DataId("data_1"), portion_size), callback);
    ASSERT_TRUE(res.has_value()) << res.error();

    while (sim::Scheduler::get_instance().tick()) {
    }

    auto& flow_ctx = flow->get_context();
    auto mplb_ctx = mplb->get_context();
    auto connection_ctx = connection->get_context();

    // because of retransmits
    ASSERT_GE(flow_ctx.delivered_size, mplb_ctx.delivered_data_size);
    ASSERT_EQ(mplb_ctx.delivered_data_size, mplb_ctx.sent_data_size);

    // because portion size might not divide packet size
    ASSERT_GE(mplb_ctx.delivered_data_size, portion_size);

    ASSERT_EQ(connection_ctx.total_data_confirmed, portion_size);

    ASSERT_EQ(count_callback_called, 1);
}

}  // namespace test
