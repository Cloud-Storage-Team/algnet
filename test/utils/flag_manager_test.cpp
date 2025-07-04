#include <gtest/gtest.h>

#include "utils/flag_manager.hpp"

namespace test {

enum TestFlagId {
    FlagA,
    FlagB,
    FlagC,
    FlagD
};

class FlagManagerTest : public ::testing::Test {
protected:
    sim::FlagManager<int> flag_manager;
    sim::Packet packet;
};

TEST_F(FlagManagerTest, RegisterFlagByAmount_Valid) {
    EXPECT_TRUE(flag_manager.register_flag_by_amount(TestFlagId::FlagA, 2));
    EXPECT_TRUE(flag_manager.register_flag_by_amount(TestFlagId::FlagB, 5));

    EXPECT_FALSE(flag_manager.register_flag_by_amount(TestFlagId::FlagC, 1));
    EXPECT_FALSE(flag_manager.register_flag_by_amount(TestFlagId::FlagA, 5));
}

TEST_F(FlagManagerTest, RegisterFlagByLength_ValidAndInvalid) {
    EXPECT_TRUE(flag_manager.register_flag_by_length(TestFlagId::FlagA, 5));
    EXPECT_FALSE(flag_manager.register_flag_by_length(TestFlagId::FlagB, 0)); 
    EXPECT_FALSE(flag_manager.register_flag_by_length(TestFlagId::FlagB, 33));

    EXPECT_TRUE(flag_manager.register_flag_by_length(TestFlagId::FlagB, 27));
    EXPECT_FALSE(flag_manager.register_flag_by_length(TestFlagId::FlagC, 1));
}

TEST_F(FlagManagerTest, RegisterFlag_Duplicate) {
    EXPECT_TRUE(flag_manager.register_flag_by_length(TestFlagId::FlagA, 5));
    EXPECT_FALSE(flag_manager.register_flag_by_length(TestFlagId::FlagA, 3));
}

TEST_F(FlagManagerTest, SetAndGetFlag_Valid) {
    EXPECT_TRUE(flag_manager.register_flag_by_length(TestFlagId::FlagA, 3));
    EXPECT_TRUE(flag_manager.register_flag_by_length(TestFlagId::FlagB, 5));

    flag_manager.set_flag(packet, TestFlagId::FlagA, 5);
    flag_manager.set_flag(packet, TestFlagId::FlagB, 17);

    EXPECT_EQ(flag_manager.get_flag(packet, TestFlagId::FlagA), 5);
    EXPECT_EQ(flag_manager.get_flag(packet, TestFlagId::FlagB), 17);
}

TEST_F(FlagManagerTest, SetGetFlag_UnregisteredFlag) {
    auto original = flag_manager.get_flag(packet, TestFlagId::FlagA);
    if (original == 1) {
        flag_manager.set_flag(packet, TestFlagId::FlagA, 0);
    } else {
        flag_manager.set_flag(packet, TestFlagId::FlagA, 1);
    }
    EXPECT_EQ(flag_manager.get_flag(packet, TestFlagId::FlagA), original);
}

TEST_F(FlagManagerTest, Reset_ClearsFlags) {
    EXPECT_TRUE(flag_manager.register_flag_by_length(TestFlagId::FlagA, 3));
    flag_manager.set_flag(packet, TestFlagId::FlagA, 7);
    EXPECT_EQ(flag_manager.get_flag(packet, TestFlagId::FlagA), 7u);

    flag_manager.reset();

    EXPECT_TRUE(flag_manager.register_flag_by_length(TestFlagId::FlagA, 3));
}

} // namespace test