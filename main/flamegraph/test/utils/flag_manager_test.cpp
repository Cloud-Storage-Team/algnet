#include "utils/flag_manager.hpp"

#include <gtest/gtest.h>

#include <random>

#include "types.hpp"

namespace test {

enum TestFlagId { FlagA, FlagB, FlagC, FlagD };

class FlagManagerTest : public ::testing::Test {
protected:
    // int here is an alias for TestFlagId
    // Can not use TestFlagId  because `fmt` used for logs in sim::FlagManager
    // does not work with enums
    sim::FlagManager<int, PacketFlagsBase> flag_manager;
    sim::BitSet<PacketFlagsBase> flags;
};

static int RANDOM_SEED = 42;

std::uint64_t random_in_range(std::mt19937_64& rnd, std::uint64_t min,
                              std::uint64_t max) {
    if (min > max) {
        throw std::invalid_argument(
            "Can not generate random number: minimal value greater than "
            "maximal");
    }
    std::uint64_t random_value = rnd();
    return (max - min == std::numeric_limits<std::uint64_t>::max()
                ? random_value
                : min + random_value % (max - min + 1));
}

std::uint64_t random_unsigned_value(std::mt19937_64& rnd, int bits_count) {
    int max_bits = sizeof_bits(std::uint64_t);
    if (bits_count > max_bits) {
        throw std::invalid_argument(
            fmt::format("Can not generate random 64bits value: given bits "
                        "count ({}) are too large",
                        bits_count));
    }
    std::uint64_t max_value =
        (bits_count == max_bits ? std::numeric_limits<std::uint64_t>::max()
                                : (1ull << bits_count) - 1);
    return random_in_range(rnd, 0, max_value);
}

template <sim::BitStorageType TBitStorage>
void test_register_by_amount() {
    int bits_count = sizeof_bits(TBitStorage);
    sim::FlagManager<int, TBitStorage> flag_manager;

    // Can not register flag with only possible value
    EXPECT_FALSE(flag_manager.register_flag_by_amount(TestFlagId::FlagA, 1));

    TBitStorage half_amont = ((TBitStorage)1 << (bits_count >> 1));

    // twicely registier half of total space
    EXPECT_TRUE(
        flag_manager.register_flag_by_amount(TestFlagId::FlagA, half_amont));
    EXPECT_TRUE(
        flag_manager.register_flag_by_amount(TestFlagId::FlagB, half_amont));

    // All bits are taken
    EXPECT_FALSE(flag_manager.register_flag_by_amount(TestFlagId::FlagB, 2));
}

TEST_F(FlagManagerTest, RegisterFlagByAmount) {
    test_register_by_amount<std::uint8_t>();
    test_register_by_amount<std::uint16_t>();
    test_register_by_amount<std::uint32_t>();
    test_register_by_amount<std::uint64_t>();
}

template <sim::BitStorageType TBitStorageType>
void test_register_flag_duplicate() {
    sim::FlagManager<int, TBitStorageType> flag_manager;
    EXPECT_TRUE(flag_manager.register_flag_by_length(TestFlagId::FlagA, 2));
    EXPECT_FALSE(flag_manager.register_flag_by_length(TestFlagId::FlagA, 2));
}

TEST_F(FlagManagerTest, RegisterDuplicateFlag) {
    test_register_flag_duplicate<std::uint8_t>();
    test_register_flag_duplicate<std::uint16_t>();
    test_register_flag_duplicate<std::uint32_t>();
    test_register_flag_duplicate<std::uint64_t>();
}

template <sim::BitStorageType TBitStirageType>
void test_set_and_get_flag() {
    std::mt19937_64 rnd(RANDOM_SEED);
    sim::FlagManager<int, TBitStirageType> flag_manager;
    sim::BitSet<TBitStirageType> flags;

    int total_bits = sizeof_bits(TBitStirageType);

    // May resitrate without chechs at least 8 bits
    int max_two_flags_sum = 7;
    int first_flag_length = random_in_range(rnd, 1, max_two_flags_sum - 1);
    int second_flag_length =
        random_in_range(rnd, 1, max_two_flags_sum - first_flag_length);
    int third_flag_length = total_bits - first_flag_length - second_flag_length;

    auto process_flag = [&rnd, &flag_manager, &flags](int flag_length) {
        static int flag_num = 0;
        EXPECT_TRUE(
            flag_manager.register_flag_by_length(flag_num, flag_length));
        TBitStirageType value = random_unsigned_value(rnd, flag_length);
        flag_manager.set_flag(flags, flag_num, value);
        EXPECT_EQ(flag_manager.get_flag(flags, flag_num), value);
        flag_num++;
    };

    process_flag(first_flag_length);
    process_flag(second_flag_length);
    process_flag(third_flag_length);
}

TEST_F(FlagManagerTest, SetAndGetFlag) {
    test_set_and_get_flag<std::uint8_t>();
    test_set_and_get_flag<std::uint16_t>();
    test_set_and_get_flag<std::uint32_t>();
    test_set_and_get_flag<std::uint64_t>();
}

template <sim::BitStorageType TBitsStorageType>
void test_set_and_get_unregistred_flag() {
    sim::FlagManager<int, TBitsStorageType> flag_manager;
    sim::BitSet<TBitsStorageType> flags;
    EXPECT_THROW(flag_manager.set_flag(flags, TestFlagId::FlagA, 0),
                 sim::FlagNotRegistratedException);
    EXPECT_THROW(flag_manager.get_flag(flags, TestFlagId::FlagA),
                 sim::FlagNotRegistratedException);
}

TEST_F(FlagManagerTest, SetGetFlag_UnregisteredFlag) {
    test_set_and_get_unregistred_flag<std::uint8_t>();
    test_set_and_get_unregistred_flag<std::uint16_t>();
    test_set_and_get_unregistred_flag<std::uint32_t>();
    test_set_and_get_unregistred_flag<std::uint64_t>();
}

template <sim::BitStorageType TBitsStorageType>
void test_get_not_set_flag() {
    sim::FlagManager<int, TBitsStorageType> flag_manager;
    sim::BitSet<TBitsStorageType> flags;
    EXPECT_TRUE(flag_manager.register_flag_by_length(TestFlagId::FlagA, 1));
    EXPECT_THROW(flag_manager.get_flag(flags, TestFlagId::FlagA),
                 sim::FlagNotSetException);
}

TEST_F(FlagManagerTest, GetFlag_NotSetFlag) {
    test_get_not_set_flag<std::uint8_t>();
    test_get_not_set_flag<std::uint16_t>();
    test_get_not_set_flag<std::uint32_t>();
    test_get_not_set_flag<std::uint64_t>();
}

template <sim::BitStorageType TBitStorageType>
void test_clear_flags() {
    sim::FlagManager<int, TBitStorageType> flag_manager;
    sim::BitSet<TBitStorageType> flags;
    std::mt19937_64 rnd(RANDOM_SEED);

    int flag_size = random_in_range(rnd, 1, sizeof_bits(TBitStorageType));
    TBitStorageType value = random_unsigned_value(rnd, flag_size);
    EXPECT_TRUE(
        flag_manager.register_flag_by_length(TestFlagId::FlagA, flag_size));
    flag_manager.set_flag(flags, TestFlagId::FlagA, value);
    EXPECT_EQ(flag_manager.get_flag(flags, TestFlagId::FlagA), value);

    flag_manager.reset();

    EXPECT_THROW(flag_manager.set_flag(flags, TestFlagId::FlagA, value),
                 sim::FlagNotRegistratedException);
    EXPECT_THROW(flag_manager.get_flag(flags, TestFlagId::FlagA),
                 sim::FlagNotRegistratedException);

    EXPECT_TRUE(
        flag_manager.register_flag_by_length(TestFlagId::FlagA, flag_size));

    flag_manager.set_flag(flags, TestFlagId::FlagA, value);
    EXPECT_EQ(flag_manager.get_flag(flags, TestFlagId::FlagA), value);
}

TEST_F(FlagManagerTest, Reset_ClearsFlags) {
    test_clear_flags<std::uint8_t>();
    test_clear_flags<std::uint16_t>();
    test_clear_flags<std::uint32_t>();
    test_clear_flags<std::uint64_t>();
}

}  // namespace test