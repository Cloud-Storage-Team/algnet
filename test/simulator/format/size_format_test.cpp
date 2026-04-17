#include <gtest/gtest.h>

#include <cmath>

#include "units/size_fmt.hpp"

TEST(SizeFormatTest, DefaultValue) {
    // default precision
    Size<Byte> byte(1ul);
    EXPECT_EQ(fmt::format("{}", byte), "1B");
}

TEST(SizeFormatTest, LargeValue) {
    // use large value
    Size<GBit> t(1000000000ul);
    EXPECT_EQ(fmt::format("{}", t), "1000000000Gb");
}

TEST(SizeFormatTest, NotDefaultPrecision) {
    Size<Bit> bit(10ul);

    EXPECT_THROW(fmt::format(fmt::runtime("{:.3}"), bit), fmt::format_error);
}
