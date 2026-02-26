#include <gtest/gtest.h>
#include <cmath>

#include "units/time_fmt.hpp"

TEST(TimeFormatTest, DefaultPrecision) {
    // default precision
    Time<Nanosecond> byte(1.23456);
    EXPECT_EQ(fmt::format("{}", byte), "1.2ns");
}

TEST(TimeFormatTest, LargeValue) {
    // use large value
    Time<Millisecond> t(1e9);
    EXPECT_EQ(fmt::format("{}", t), "1000000000.0ms");
}

TEST(TimeFormatTest, CustomPrecision) {
    // specify precision
    Time<Second> t(1.23456);
    EXPECT_EQ(fmt::format("{:.1f}", t), "1.2s");
    EXPECT_EQ(fmt::format("{:.4f}", t), "1.2346s");
}