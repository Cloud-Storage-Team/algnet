#include <gtest/gtest.h>

#include <cmath>

#include "units/time_fmt.hpp"

TEST(TimeFormatTest, DefaultPrecision) {
    // default precision
    Time<Nanosecond> ns(1);
    EXPECT_EQ(fmt::format("{}", ns), "1.0ns");
}

TEST(TimeFormatTest, CustomPrecision) {
    // custom precision
    Time<Microsecond> t(Time<Nanosecond>(230));
    EXPECT_EQ(fmt::format("{}", t), "0.2us");
    EXPECT_EQ(fmt::format("{:.2f}", t), "0.23us");
}
