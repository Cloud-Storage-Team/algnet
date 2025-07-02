#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <vector>

#include "utils/bitset.hpp"

namespace test {

class BitSet32Test : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};


TEST(BitSet32Test, InvalidCases) {
    sim::BitSet32 bits(0x12345678);
    const uint32_t original = bits.get_bits();

    bits.set_range(0, 32, 15);
    EXPECT_EQ(bits.get_bits(), original);
    
    bits.set_range(32, 35, 15);  
    EXPECT_EQ(bits.get_bits(), original);
    
    bits.set_range(10, 9, 15);   
    EXPECT_EQ(bits.get_bits(), original);
    
    bits.set_range(100, 200, 15);
    EXPECT_EQ(bits.get_bits(), original);

    bits.set_range(5, 5, 256);
    EXPECT_EQ(bits.get_bits(), original);

    bits.set_range(5, 6, 4);
    EXPECT_EQ(bits.get_bits(), original);
}

TEST(BitSet32Test, BasicCases) {
    sim::BitSet32 bits(0);

    bits.set_range(4, 4, 1);
    EXPECT_EQ(bits.get_range(4, 4), bits.get_bit(4));
    EXPECT_EQ(bits.get_range(4, 4), 1);

    bits.set_bit(5, 1);
    EXPECT_EQ(bits.get_range(5, 5), bits.get_bit(5));
    EXPECT_EQ(bits.get_range(5, 5), 1);
    
    bits.set_range(4, 8, 20);
    EXPECT_EQ(bits.get_range(4, 8), 20);
    
    bits.set_range(0, 31, 137);
    EXPECT_EQ(bits.get_range(0, 31), 137);
}

TEST(BitSet32Test, ValidEdgeCases) {
    sim::BitSet32 bits(0);
    
    bits.set_bit(0, 1);
    EXPECT_EQ(bits.get_bit(0), bits.get_range(0, 0));
    EXPECT_EQ(bits.get_bit(0), 1);

    bits.set_range(0, 0, 0);
    EXPECT_EQ(bits.get_bit(0), bits.get_range(0, 0));
    EXPECT_EQ(bits.get_bit(0), 0);
    
    bits.set_range(31, 31, 1);
    EXPECT_EQ(bits.get_bit(31), bits.get_range(31, 31));
    EXPECT_EQ(bits.get_bit(31), 1);

    bits.set_bit(31, 0);
    EXPECT_EQ(bits.get_bit(31), bits.get_range(31, 31));
    EXPECT_EQ(bits.get_bit(31), 0);

    bits.set_range(8, 15, 0xFF);
    EXPECT_EQ(bits.get_range(8, 15), 0xFF);

    bits.set_range(0, 31, 0xFFFFFFFF);
    EXPECT_EQ(bits.get_bits(), 0xFFFFFFFF);

    bits.set_range(0, 7, 0);
    EXPECT_EQ(bits.get_range(0, 7), 0);

    bits.set_range(0, 31, 0);
    EXPECT_EQ(bits.get_bits(), 0);
}

} // namespace test