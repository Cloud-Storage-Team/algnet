#include "connection/new_connection.hpp"

#include <gtest/gtest.h>

#include "../_mocks/new_mplb_mock.hpp"

namespace test {
class NewConnectionSendTest : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(NewConnectionSendTest, CorrectSend) {
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

}  // namespace test