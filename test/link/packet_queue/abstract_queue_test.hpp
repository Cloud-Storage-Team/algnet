#pragma once
#include <gtest/gtest.h>

#include "link/packet_queue/i_packet_queue.hpp"

namespace test {

template <typename TPacketQueue, typename... Args>
void TestPopEmpty(Args&&... args) {
    static_assert(std::is_base_of_v<sim::IPacketQueue, TPacketQueue>,
                  "TPacketQueue must be inherit from IPacketQueue");
    static_assert(std::is_constructible_v<TPacketQueue, Args&&...>,
                  "TPacketQueue must be constructable from given args");

    TPacketQueue queue(std::forward<Args>(args)...);

    bool runtime_error_catched = false;
    try {
        queue.pop();
    } catch (const std::exception& e) {
        runtime_error_catched = true;
    }
    ASSERT_TRUE(runtime_error_catched);
}

}  // namespace test