#pragma once
#include <cstdint>
#include <string>

#include "logger/logger.hpp"
#include "packet/packet.hpp"
namespace test {

struct FakePacket : public sim::Packet {
    FakePacket(std::shared_ptr<sim::IDevice> device) : sim::Packet() {
        route.dest.device_id = device->get_id();
    }
};

}  // namespace test
