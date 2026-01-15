#include "connection/new_connection.hpp"

#include <gtest/gtest.h>

#include "../_mocks/new_mplb_mock.hpp"

namespace test {
class NewConnectionSendTest : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(NewConnectionSendTest, SimpleSend) {
    auto mplb = std::make_shared<MplbMock>(SizeByte(100));
    auto connection = sim::NewConnection::create("", mplb);

    bool delivered = false;

    if (auto res =
            connection->send_data(sim::Data(sim::DataId(), SizeByte(100)),
                                  [&]() { delivered = true; });
        !res.has_value()) {
        FAIL() << res.error();
    }

    ASSERT_TRUE(delivered);
}

TEST_F(NewConnectionSendTest, SendManyPortions) {
    auto mplb = std::make_shared<MplbMock>(SizeByte(10));
    auto connection = sim::NewConnection::create("", mplb);

    bool delivered = false;

    if (auto res =
            connection->send_data(sim::Data(sim::DataId(), SizeByte(100)),
                                  [&]() { delivered = true; });
        !res.has_value()) {
        FAIL() << res.error();
    }

    ASSERT_TRUE(delivered);
}

TEST_F(NewConnectionSendTest, IncorrectSend) {
    auto mplb = std::make_shared<MplbMock>(SizeByte(0));
    auto connection = sim::NewConnection::create("", mplb);

    bool delivered = false;

    auto res = connection->send_data(sim::Data(sim::DataId(), SizeByte(100)),
                                     [&]() { delivered = true; });
    ASSERT_TRUE(res.has_value()) << res.error();
    ASSERT_FALSE(delivered)
        << "On delivery callback should not be triggered, but it did";
}

TEST_F(NewConnectionSendTest, SendRepeatingDataIds) {
    auto mplb = std::make_shared<MplbMock>(SizeByte(100));
    auto connection = sim::NewConnection::create("", mplb);

    sim::DataId id("id");
    {
        bool delivered = false;

        auto res = connection->send_data(sim::Data(id, SizeByte(100)),
                                         [&]() { delivered = true; });
        ASSERT_TRUE(res.has_value()) << res.error();
        ASSERT_TRUE(delivered) << "Data should be delivered, but it was not";
    }
    {
        bool delivered = false;

        auto res = connection->send_data(sim::Data(id, SizeByte(100)),
                                         [&]() { delivered = true; });
        ASSERT_FALSE(res.has_value())
            << "Sending portion of data with repeating id should fail";
        ASSERT_FALSE(delivered)
            << "On delivery callback should not be triggered, but it did";
    }
}

TEST_F(NewConnectionSendTest, SendManyIds) {
    auto mplb = std::make_shared<MplbMock>(SizeByte(100));
    auto connection = sim::NewConnection::create("", mplb);

    for (int num = 0; num < 3; num++) {
        sim::DataId id(fmt::format("id_{}", num));
        bool delivered = false;

        auto res = connection->send_data(sim::Data(id, SizeByte(100)),
                                         [&]() { delivered = true; });
        ASSERT_TRUE(res.has_value()) << res.error();
        ASSERT_TRUE(delivered) << fmt::format(
            "Data {} should be delivered, but it was not", id.to_string());
    }
}

}  // namespace test