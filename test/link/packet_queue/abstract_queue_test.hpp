#pragma once
#include <gtest/gtest.h>

#include "link/packet_queue/i_packet_queue.hpp"

namespace test {

template <typename TPacketQueue, typename... Args>
concept ValidPacketQueueWithArgs =
    std::is_base_of_v<sim::IPacketQueue, TPacketQueue> &&
    std::is_constructible_v<TPacketQueue, Args&&...>;

template <typename TPacketQueue, typename... Args>
requires ValidPacketQueueWithArgs<TPacketQueue, Args...>
void TestEmpty(Args&&... args) {
    TPacketQueue queue(std::forward<Args>(args)...);

    ASSERT_TRUE(queue.empty());
    ASSERT_EQ(queue.get_size(), SizeByte(0));

    bool runtime_error_catched = false;
    try {
        queue.pop();
    } catch (const std::exception& e) {
        runtime_error_catched = true;
    }
    ASSERT_TRUE(runtime_error_catched);

    runtime_error_catched = false;
    try {
        (void)queue.front();
    } catch (const std::exception& e) {
        runtime_error_catched = true;
    }
    ASSERT_TRUE(runtime_error_catched);
}

template <typename TPacketQueue, typename... Args>
requires ValidPacketQueueWithArgs<TPacketQueue, Args...>
void TestPushOnePacket(SizeByte packet_size, Args&&... args) {
    TPacketQueue queue(std::forward<Args>(args)...);

    sim::Packet packet(packet_size);

    ASSERT_TRUE(queue.push(packet));

    ASSERT_EQ(queue.front(), packet);
    ASSERT_FALSE(queue.empty());
    ASSERT_EQ(queue.get_size(), packet.size);

    queue.pop();
    ASSERT_EQ(queue.get_size(), SizeByte(0));
    ASSERT_TRUE(queue.empty());
}

template <typename TPacketQueue, typename... Args>
requires ValidPacketQueueWithArgs<TPacketQueue, Args...>
void TestOverflow(Args&&... args) {
    TPacketQueue queue(std::forward<Args>(args)...);

    const SizeByte max_size = queue.get_max_size();
    const size_t NUMBER_OF_PACKETS = 10;
    SizeByte packet_size = Size<Bit>(max_size.get_bits() / NUMBER_OF_PACKETS);

    sim::Packet packet(packet_size);

    for (size_t i = 0; i < NUMBER_OF_PACKETS; i++) {
        ASSERT_TRUE(queue.push(packet));
    }

    SizeByte expected_queue_size = packet_size * NUMBER_OF_PACKETS;
    ASSERT_EQ(queue.get_size(), expected_queue_size);

    sim::Packet overflow_packet(packet_size + SizeByte(1));
    ASSERT_FALSE(queue.push(overflow_packet));

    ASSERT_EQ(queue.get_size(), expected_queue_size);

    for (size_t i = 0; i < NUMBER_OF_PACKETS; i++) {
        ASSERT_EQ(queue.front(), packet);
        queue.pop();
    }

    ASSERT_TRUE(queue.empty());
    ASSERT_EQ(queue.get_size(), SizeByte(0));
}

}  // namespace test