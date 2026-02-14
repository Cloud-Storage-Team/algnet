#include <gtest/gtest.h>

#include <cmath>

#include "units/speed_fmt.hpp"

class SpeedFormatTest : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(SpeedFormatTest, DefaultPrecision) {
    // default precision
    Speed<Byte, Second> byte(1.23456);
    EXPECT_EQ(fmt::format("{}", byte), "1.2B/s");
}

TEST_F(SpeedFormatTest, LargeValue) {
    // use large value
    Speed<GBit, Millisecond> t(1000000);
    EXPECT_EQ(fmt::format("{}", t), "1000000.0Gb/ms");
}

TEST_F(SpeedFormatTest, CustomPrecision) {
    // specify precision
    Speed<MByte, Nanosecond> t(1.23456);
    EXPECT_EQ(fmt::format("{:.1f}", t), "1.2MB/ns");
    EXPECT_EQ(fmt::format("{:.4f}", t), "1.2346MB/ns");
}