#include <gtest/gtest.h>

#include <cmath>

#include "units/speed_fmt.hpp"

TEST(SpeedFormatTest, DefaultPrecision) {
    // default precision
    Speed<Byte, Second> byte(1.23456);
    EXPECT_EQ(fmt::format("{}", byte), "1.2Bps");
}

TEST(SpeedFormatTest, LargeValue) {
    // use large value
    Speed<GBit, Millisecond> t(1000000);
    EXPECT_EQ(fmt::format("{}", t), "1000000.0Gbpms");
}

TEST(SpeedFormatTest, CustomPrecision) {
    // specify precision
    Speed<MByte, Nanosecond> t(1.23456);
    EXPECT_EQ(fmt::format("{:.1f}", t), "1.2MBpns");
    EXPECT_EQ(fmt::format("{:.4f}", t), "1.2346MBpns");
}