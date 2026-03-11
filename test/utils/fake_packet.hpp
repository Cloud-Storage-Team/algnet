#pragma once
#include <cstdint>
#include <string>

#include "device/interfaces/i_device.hpp"
#include "logger/logger.hpp"
#include "packet.hpp"

namespace test {

struct FakePacket : public sim::Packet {
    FakePacket(std::shared_ptr<sim::IDevice> device) {
        receiver_id = device->get_id();
    };
};

}  // namespace test
