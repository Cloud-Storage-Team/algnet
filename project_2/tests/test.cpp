#include <gtest/gtest.h>
#include "test_utils.hpp"

TEST(ExampleTest, AddsTwoNumbers) {
    std::uint64_t switch_buffer_size = 819200;
    std::string result = RunSimulation<ReceiverPacketNumber, Sender, std::uint64_t, std::uint64_t>(1, 1, switch_buffer_size, 0, senderFactory<std::uint64_t, std::uint64_t>, 0, 1000);
    EXPECT_EQ(result, "0");
    result = RunSimulation<ReceiverPacketNumber, Sender, std::uint64_t, std::uint64_t>(1, 1000, switch_buffer_size, 0, senderFactory<std::uint64_t, std::uint64_t>, 0, 1000);
    EXPECT_EQ(result, "1");
    result = RunSimulation<ReceiverPacketNumber, Sender, std::uint64_t, std::uint64_t>(1, 1500, switch_buffer_size, 0, senderFactory<std::uint64_t, std::uint64_t>, 0, 1000);
    EXPECT_EQ(result, "2");
    result = RunSimulation<ReceiverPacketNumber, Sender, std::uint64_t, std::uint64_t>(1, 10000, switch_buffer_size, 0, senderFactory<std::uint64_t, std::uint64_t>, 0, 1000);
    EXPECT_EQ(result, "10");
    
    result = RunSimulation<ReceiverPacketNumber, Sender, std::uint64_t, std::uint64_t>(2, 1000, switch_buffer_size, 0, senderFactory<std::uint64_t, std::uint64_t>, 0, 1000);
    EXPECT_EQ(result, "2");
    result = RunSimulation<ReceiverPacketNumber, Sender, std::uint64_t, std::uint64_t>(2, 1500, switch_buffer_size, 0, senderFactory<std::uint64_t, std::uint64_t>, 0, 1000);
    EXPECT_EQ(result, "4");
    result = RunSimulation<ReceiverPacketNumber, Sender, std::uint64_t, std::uint64_t>(5, 20000, switch_buffer_size, 0, senderFactory<std::uint64_t, std::uint64_t>, 0, 1000);
    EXPECT_EQ(result, "100");

    result = RunSimulation<ReceiverPacketNumber, Sender, std::uint64_t, std::uint64_t>(2, 5000, 8192, 300, senderFactory<std::uint64_t, std::uint64_t>, 700, 500);
    EXPECT_EQ(result, "9");
    result = RunSimulation<ReceiverPacketNumber, Sender, std::uint64_t, std::uint64_t>(3, 5000, 8192, 300, senderFactory<std::uint64_t, std::uint64_t>, 700, 500);
    EXPECT_EQ(result, "12");


    // result = RunSimulation<ReceiverPacketNumber, Sender, std::uint64_t, std::uint64_t>(1, 1000, switch_buffer_size, 0, senderFactory<std::uint64_t, std::uint64_t>, 0, 10);
    // EXPECT_EQ(result, "6");
}
