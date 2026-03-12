#pragma once
#include <cstdint>
#include <string>

#include "logger/logger.hpp"
#include "packet.hpp"
#include "topology/device/interfaces/i_device.hpp"

namespace test {

struct FakePacket : public sim::Packet {
    FakePacket(std::shared_ptr<sim::IDevice> device) {
        receiver_id = device->get_id();
    };
};

}  // namespace test
