#include <gtest/gtest.h>
#include <cmath>

#include "units/size_fmt.hpp"

class SizeFormatTest: public testing::Test{
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(SizeFormatTest, DefaultValue) {
    // default precision
    Size<Byte> byte(1);
    EXPECT_EQ(fmt::format("{}", byte), "1B");
}

TEST_F(SizeFormatTest, LargeValue) {
    // use large value
    Size<GBit> t(1000000000ULL);
    EXPECT_EQ(fmt::format("{}", t), "1000000000Gb");
}
